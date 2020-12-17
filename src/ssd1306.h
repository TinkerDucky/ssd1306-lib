#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

#include "ssd1306_enum.h"

static const uint8_t init_cmds[] = {
    COMMANDS::SET_DISPLAY_OFF,
    COMMANDS::SET_DISPLAY_CLK_DIVIDER_CONTROL,
    COMMANDS::SET_DISPLAY_CLK_DIVIDER_128,
    COMMANDS::SET_MULTIPLEX_RATIO_CONTROL,
    COMMANDS::SET_MULTIPLEX_RATIO_64,
    COMMANDS::SET_DISPLAY_OFFSET_CONTROL,
    0x00,  // no offset
    COMMANDS::SET_START_LINE | 0x00,
    COMMANDS::SET_CHARGE_BUMP_CONTROL,
    COMMANDS::SET_CHARGE_BUMP_ON,
    COMMANDS::SET_SEG_REMAP_ON,
    COMMANDS::SET_COM_SCAN_DECREMENT,
    COMMANDS::SET_COM_PINS_CONFIG_CONTROL,
    COMMANDS::SET_COM_PINS_ALT_NO_REMAP,
    COMMANDS::SET_CONTRAST_CONTROL,
    COMMANDS::SET_CONTRAST_207,
    COMMANDS::SET_PRECHARGE_CONTROL,
    COMMANDS::SET_PRECHARGE_PHASE1_1_PHASE2_15,
    COMMANDS::SET_VCOM_DESELECT_LEVEL_CONTROL,
    COMMANDS::SET_VCOM_DESELECT_LEVEL_07VCC,
    COMMANDS::SET_MEMORY_ADDRESSING_MODE_CONTROL,
    COMMANDS::SET_HORIZONTAL_MEMORY_ADDRESSING_MODE,
    COMMANDS::SET_PAGE_ADDR_CONTROL,
    0x00,  // page start address = 0
    0x07,  // page end address = 7;
    COMMANDS::SET_COLUMN_ADDR,
    0x00,  // column start address = 0
    0x7F,  // column end address = 127
    COMMANDS::SET_DISPLAY_BY_RAM,
    COMMANDS::SET_NORMAL_DISPLAY,
    COMMANDS::SET_DEACTIVATE_SCROLL,
    COMMANDS::SET_DISPLAY_ON,
};

template <uint8_t WIDTH, uint8_t HEIGHT, uint8_t PPB,
          int (*i2c_write)(const char* buf, uint32_t len)>
class SSD1306 {
 private:
  uint8_t framebuffer[(WIDTH * HEIGHT) / PPB];
  uint8_t current_page, current_column;

  uint8_t command(const uint8_t command) {
    char buf[2] = {CONTROL_BYTE_COMMANDS::SEND_SINGLE_CMD, command};
    return i2c_write(buf, 2);
  }

  uint8_t commandLine(const uint8_t* command_buf, uint8_t len) {
    char buf[len] = {CONTROL_BYTE_COMMANDS::SEND_MULTIPLE_CMDS};
    memcpy(buf + 1, command_buf, len);
    return i2c_write(buf, len + 1);
  }

  uint8_t data(const uint8_t data) {
    char buf[2] = {CONTROL_BYTE_COMMANDS::SEND_SINGLE_DATA, data};
    return i2c_write(buf, 2);
  }

  uint8_t multipleData(const uint8_t* data_buf, uint8_t len) {
    char buf[len] = {CONTROL_BYTE_COMMANDS::SEND_MULTIPLE_DATA};
    memcpy(buf + 1, data_buf, len);
    return i2c_write(buf, len + 1);
  }

 public:
  SSD1306() : framebuffer{0}, current_page(0), current_column(0) {}

  void init() { this->commandLine(init_cmds, sizeof(init_cmds)); }

  void showDisplay() {
    char buf_out[(WIDTH * HEIGHT) / PPB + 1] = {
        CONTROL_BYTE_COMMANDS::SEND_MULTIPLE_DATA};

    if (this->current_column != 0 || this->current_page != 0) {
      this->setPosition(0, 0);
    }

    memcpy(buf_out + 1, this->framebuffer, (WIDTH * HEIGHT) / PPB);
    i2c_write(buf_out, sizeof(buf_out));

    if (this->current_column != 0 || this->current_page != 0) {
      this->setPosition(this->current_page, this->current_column);
    }
  }

  void setPosition(uint8_t page, uint8_t column) {
    const uint8_t cmds[] = {
        COMMANDS::SET_PAGE_ADDR_CONTROL,
        page &
            ((HEIGHT / PPB) - 1),  // page start address = page % (height/ppb)
        0x07,                      // page end address = 7;
        COMMANDS::SET_COLUMN_ADDR,
        column & (WIDTH - 1),  // column start address = column % width
        0x7F,                  // column end address = 127
    };

    if (this->commandLine(cmds, sizeof(cmds)) == 0) {
      this->current_page = cmds[1];
      this->current_column = cmds[4];
    }
  }

  void setAndShowByteAtCurrentPosition(const uint8_t data) {
    if (this->data(data) != 0) return;

    this->framebuffer[this->current_column + (this->current_page * WIDTH)] =
        data;

    this->current_column = (++this->current_column) & (WIDTH - 1);
    this->current_page = (this->current_column != 0)
                             ? this->current_page
                             : (this->current_page + 1) & ((HEIGHT / PPB) - 1);
  }

  void setAndShowBytesFromCurrentPosition(const uint8_t* data_buf,
                                          uint8_t len) {
    if (this->multipleData(data_buf, len) != 0) return;

    memcpy((this->framebuffer + this->current_column +
            (this->current_page * WIDTH)),
           data_buf, len);

    this->current_page =
        (this->current_page + (this->current_column + len) / WIDTH) &
        ((HEIGHT / PPB) - 1);
    this->current_column = (this->current_column + len) & (WIDTH - 1);
  }

  void setPixel(uint8_t pos_x, uint8_t pos_y, uint8_t value) {
    if (pos_x >= WIDTH) return;
    if (pos_y >= HEIGHT) return;

    this->framebuffer[pos_x + (pos_y / PPB) * WIDTH] =
        (this->framebuffer[pos_x + (pos_y / PPB) * WIDTH] &
         ~(1 << pos_y % PPB)) |
        (value << pos_y % PPB);
  }

  void clearDisplay() {
    for (uint16_t i = 0; i < sizeof(framebuffer); i++) {
      this->framebuffer[i] = 0;
    }
    this->showDisplay();
  }

  void showTestGrid() {
    for (uint16_t i = 0; i < sizeof(this->framebuffer) * PPB; i++) {
      this->framebuffer[(i / PPB)] =
          (i % 2 && (i / PPB) % 2)
              ? this->framebuffer[i / PPB] & ~(1 << (i % PPB))
              : this->framebuffer[i / PPB] | 1 << (i % PPB);
    }
    this->showDisplay();
  }
};
#endif /* SSD1306_H */

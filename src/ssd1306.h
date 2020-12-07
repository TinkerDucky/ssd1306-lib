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

template <uint8_t WIDTH, uint8_t HEIGHT, uint8_t PPR,
          int (*i2c_write)(const char* buf, uint32_t len)>
class SSD1306 {
 private:
  uint8_t framebuffer[(WIDTH * HEIGHT) / PPR];

  void command(const char command) {
    char buf[2] = {CONTROL_BYTE_COMMANDS::SEND_SINGLE_CMD, command};
    i2c_write(buf, 2);
  }

  void commandLine(const uint8_t* command_buf, uint8_t len) {
    char buf[len] = {CONTROL_BYTE_COMMANDS::SEND_MULTIPLE_CMDS};
    memcpy(buf + 1, command_buf, len);
    i2c_write(buf, len + 1);
  }

 public:
  SSD1306() : framebuffer{0} {}

  void init() { this->commandLine(init_cmds, sizeof(init_cmds)); }

  void showDisplay() {
    char buf_out[(WIDTH * HEIGHT) / PPR + 1] = {
        CONTROL_BYTE_COMMANDS::SEND_MULTIPLE_DATA};
    memcpy(buf_out + 1, this->framebuffer, (WIDTH * HEIGHT) / PPR);
    i2c_write(buf_out, sizeof(buf_out));
  }

  void setPixel(uint8_t pos_x, uint8_t pos_y, uint8_t value) {
    if (pos_x >= WIDTH) return;
    if (pos_y >= HEIGHT) return;

    this->framebuffer[pos_x + (pos_y / PPR) * WIDTH] =
        (this->framebuffer[pos_x + (pos_y / PPR) * WIDTH] &
         ~(1 << pos_y % PPR)) |
        (value << pos_y % PPR);
  }

  void clearDisplay() {
    for (uint16_t i = 0; i < sizeof(framebuffer); i++) {
      this->framebuffer[i] = 0;
    }
    this->showDisplay();
  }

  void showTestGrid() {
    for (uint16_t i = 0; i < sizeof(this->framebuffer) * PPR; i++) {
      this->framebuffer[(i / PPR)] =
          (i % 2 && (i / PPR) % 2)
              ? this->framebuffer[i / PPR] & ~(1 << (i % PPR))
              : this->framebuffer[i / PPR] | 1 << (i % PPR);
    }
    this->showDisplay();
  }
};
#endif /* SSD1306_H */

#include <bcm2835.h>

#include <cstring>
#include <iostream>

#include "ssd1306.h"

const uint8_t ADDR = 0x3C;
const uint8_t WIDTH = 128;
const uint8_t HEIGHT = 64;
const uint8_t PIXEL_PER_ROW = 8;

int main() {
  if (!bcm2835_init()) {
    printf("bcm2835_init failed. Are you running as root??\n");
    return 1;
  }
  printf("bcm2835_init successfull\n");
  bcm2835_i2c_begin();
  bcm2835_i2c_setSlaveAddress(ADDR);
  // bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_2500);

  SSD1306<WIDTH, HEIGHT, PIXEL_PER_ROW, bcm2835_i2c_write> display;
  display.init();
  display.clearDisplay();
  bcm2835_delay(1000);
  display.showTestGrid();

  bcm2835_i2c_end();
}
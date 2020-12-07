# SSD1306 Display Library
This lib shall be a lightweight alternative to other ssd1306 libs by using templates to generate only the necessary code depending on the used platform.

# Usage
Currently the lib can be used by including ```ssd1306.h``` and creating a variable of type ```SSD1306<uint8_t WIDTH, uint8_t HEIGHT, uint8_t PPR, int (*i2c_write)(const char* buf, uint32_t len)>``` where ```WIDTH``` and ```HEIGHT``` are the display dimensions, ```PPR``` the pixel per row number (will be renamed in pixel per page, are mostly 8) and ```(*i2c_write)(const char* buf, uint32_t len)>``` a function pointer to a function which writes on the I2C-Bus (which has been initialised before).


# Examples
## RaspberryPi with bcm2835-lib and 128x64 Monochrome Display 

```cpp
#include <bcm2835.h>
const uint8_t ADDR = 0x3C;
const uint8_t WIDTH = 128;
const uint8_t HEIGHT = 64;
const uint8_t PIXEL_PER_ROW = 8;

bcm2835_i2c_begin();
bcm2835_i2c_setSlaveAddress(ADDR);

SSD1306<WIDTH, HEIGHT, PIXEL_PER_ROW, bcm2835_i2c_write> display;

display.init();
display.clearDisplay();
bcm2835_delay(1000);
display.showTestGrid();

bcm2835_i2c_end();

```
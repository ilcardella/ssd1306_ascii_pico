#include <hardware/i2c.h>
#include <pico/stdlib.h>

#include <ssd1306_ascii_pico/ssd1306_ascii_i2c.h>

int main()
{
    I2CBus bus = I2CBus(i2c0, 4, 5, 400000L);
    SSD1306AsciiI2C display = SSD1306AsciiI2C(bus);

    display.begin(&Adafruit128x64, 0x3C);
    display.setFont(Adafruit5x7);
    display.clear();
    display.setCursor(0, 0);
    display.print("Hello World!");

    return 0;
}

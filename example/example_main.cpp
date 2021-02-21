#include <pico/stdlib.h>
#include <ssd1306_ascii_pico/ssd1306_ascii_i2c.h>

int main()
{
    Test test = Test();
    test.test_method();
    return 0;
}

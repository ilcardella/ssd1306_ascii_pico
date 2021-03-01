#pragma once

#include "SSD1306Ascii.h"

#include <hardware/i2c.h>
#include <pico/binary_info.h>

class I2CBus
{
  public:
    I2CBus(const uint8_t &port_id, const unsigned char &sda_pin,
           const unsigned char &scl_pin, const unsigned long speed)
    {
        port = port_id == 0 ? i2c0 : i2c1;
        i2c_init(port, speed);
        gpio_set_function(sda_pin, GPIO_FUNC_I2C);
        gpio_set_function(scl_pin, GPIO_FUNC_I2C);
        gpio_pull_up(sda_pin);
        gpio_pull_up(scl_pin);
        // Make the I2C pins available to picotool
        bi_decl(bi_2pins_with_func(sda_pin, scl_pin, GPIO_FUNC_I2C));
    }

    void setClock(const long &speed)
    {
        i2c_set_baudrate(port, speed);
    }

    void endTransmission()
    {
        // Nothing to do
    }

    void beginTransmission(const uint8_t &addr)
    {
        address = addr;
    }

    void write(const uint8_t &c)
    {
        i2c_write_blocking(port, address, &c, 1, false);
    }

  private:
    uint8_t address = 0x00;
    i2c_inst_t *port = nullptr;
};

/**
 * @class SSD1306AsciiI2C
 * @brief Class for I2C displays.
 */
class SSD1306AsciiI2C : public SSD1306Ascii
{
  public:
    /**
     * @brief Initialize object on specific I2C bus.
     *
     * @param[in] bus The I2C bus to be used.
     */
    explicit SSD1306AsciiI2C(I2CBus &bus) : i2c_bus(bus)
    {
    }
    /**
     * @brief Initialize the display controller.
     *
     * @param[in] dev A device initialization structure.
     * @param[in] i2cAddr The I2C address of the display controller.
     */
    void begin(const DevType *dev, uint8_t i2cAddr)
    {
#if OPTIMIZE_I2C
        m_nData = 0;
#endif // OPTIMIZE_I2C
        m_i2cAddr = i2cAddr;
        init(dev);
    }
    /**
     * @brief Initialize the display controller.
     *
     * @param[in] dev A device initialization structure.
     * @param[in] i2cAddr The I2C address of the display controller.
     * @param[in] rst The display controller reset pin.
     */
    void begin(const DevType *dev, uint8_t i2cAddr, uint8_t rst)
    {
        oledReset(rst);
        begin(dev, i2cAddr);
    }

  protected:
    void writeDisplay(uint8_t b, uint8_t mode)
    {
#if OPTIMIZE_I2C
        if (m_nData > 16 || (m_nData && mode == SSD1306_MODE_CMD))
        {
            i2c_bus.endTransmission();
            m_nData = 0;
        }
        if (m_nData == 0)
        {
            i2c_bus.beginTransmission(m_i2cAddr);
            i2c_bus.write(mode == SSD1306_MODE_CMD ? 0X00 : 0X40);
        }
        i2c_bus.write(b);
        if (mode == SSD1306_MODE_RAM_BUF)
        {
            m_nData++;
        }
        else
        {
            i2c_bus.endTransmission();
            m_nData = 0;
        }
#else  // OPTIMIZE_I2C
        i2c_bus.beginTransmission(m_i2cAddr);
        i2c_bus.write(mode == SSD1306_MODE_CMD ? 0X00 : 0X40);
        i2c_bus.write(b);
        i2c_bus.endTransmission();
#endif // OPTIMIZE_I2C
    }

  protected:
    I2CBus &i2c_bus;
    uint8_t m_i2cAddr;
#if OPTIMIZE_I2C
    uint8_t m_nData;
#endif // OPTIMIZE_I2C
};

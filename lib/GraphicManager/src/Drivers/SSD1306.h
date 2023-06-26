#include "IGraphicDriver.h"
#include "driver/i2c.h"

#ifndef SSD1306_GUARD
#define SSD1306_GUARD

class SSD1306 : public IGraphicDriver{
    public:
    esp_err_t Init(void);
    void UpdatePage(void);
    private:
    i2c_config_t i2c_config;
    const uint8_t I2C_MASTER_SDA_IO = 21;
    const uint8_t I2C_MASTER_SCL_IO = 22;
    const int I2C_MASTER_FREQ_HZ = 400000;
};

#endif /* SSD1306_GUARD */
#include "SSD1306.h"

#include "driver/gpio.h"
#include "string.h"

esp_err_t SSD1306::Init(void)
{
    esp_err_t ret = ESP_OK;

    memset(&this->i2c_config, 0, sizeof(i2c_config_t));

    this->i2c_config.mode = I2C_MODE_MASTER;
    this->i2c_config.sda_io_num = this->I2C_MASTER_SDA_IO;
    this->i2c_config.scl_io_num = this->I2C_MASTER_SCL_IO;
    this->i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    this->i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    this->i2c_config.master.clk_speed = this->I2C_MASTER_FREQ_HZ;
    this->i2c_config.clk_flags = 0;

    ret += i2c_param_config(I2C_NUM_0, &i2c_config);
    ret += i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

    return ret;
}

void SSD1306::UpdatePage(void)
{
    // https://embeddedexplorer.com/esp32-i2c-tutorial/
    // https://github.com/yanbe/ssd1306-esp-idf-i2c/blob/master/main/main.c
}

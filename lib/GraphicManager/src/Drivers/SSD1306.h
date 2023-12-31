#include "IGraphicDriver.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#ifndef SSD1306_GUARD
#define SSD1306_GUARD

/**
 * SSD1306 is a class that represents a driver for the SSD1306 OLED display.
 * It inherits from the IGraphicDriver interface.
 */
class SSD1306 : public IGraphicDriver
{
public:
  esp_err_t Initialize(void);
  void Process(uint8_t *memory_area_data);

private:
  i2c_config_t i2c_config;

  /* Private functions  */
private:
  esp_err_t SendInitializationCommands_(void);
  esp_err_t ClearDisplay_(void);
  esp_err_t PrintPattern_(uint8_t *pattern);
  esp_err_t SetupPage_(int page);
  void SendResetCommand_(void);

  /* Private constants */
private:
  const gpio_num_t OLED_I2C_MASTER_SDA_IO = GPIO_NUM_4;
  const gpio_num_t OLED_I2C_MASTER_SCL_IO = GPIO_NUM_15;
  const gpio_num_t OLED_I2C_RESET_GPIO = GPIO_NUM_16;
  const int OLED_I2C_MASTER_FREQ_HZ = 400000;

  const uint8_t OLED_I2C_ADDRESS = 0x3C;

  /* Control byte */
  const uint8_t OLED_CONTROL_BYTE_CMD_SINGLE = 0x80;
  const uint8_t OLED_CONTROL_BYTE_CMD_STREAM = 0x00;
  const uint8_t OLED_CONTROL_BYTE_DATA_STREAM = 0x40;

  /* Fundamental commands (pg.28) */
  const uint8_t OLED_CMD_SET_CONTRAST = 0x81;
  const uint8_t OLED_CMD_DISPLAY_RAM = 0xA4;
  const uint8_t OLED_CMD_DISPLAY_ALLON = 0xA5;
  const uint8_t OLED_CMD_DISPLAY_NORMAL = 0xA6;
  const uint8_t OLED_CMD_DISPLAY_INVERTED = 0xA7;
  const uint8_t OLED_CMD_DISPLAY_OFF = 0xAE;
  const uint8_t OLED_CMD_DISPLAY_ON = 0xAF;

  /* Addressing Command Table (pg.30) */
  const uint8_t OLED_CMD_SET_MEMORY_ADDR_MODE = 0x20;
  const uint8_t OLED_CMD_SET_COLUMN_RANGE = 0x21;
  const uint8_t OLED_CMD_SET_PAGE_RANGE = 0x22;

  /* Hardware Config (pg.31) */
  const uint8_t OLED_CMD_SET_DISPLAY_START_LINE = 0x40;
  const uint8_t OLED_CMD_SET_SEGMENT_REMAP = 0xA1;
  const uint8_t OLED_CMD_SET_MUX_RATIO = 0xA8;
  const uint8_t OLED_CMD_SET_COM_SCAN_MODE = 0xC8;
  const uint8_t OLED_CMD_SET_DISPLAY_OFFSET = 0xD3;
  const uint8_t OLED_CMD_SET_COM_PIN_MAP = 0xDA;
  const uint8_t OLED_CMD_NOP = 0xE3;

  /* Timing and Driving Scheme (pg.32) */
  const uint8_t OLED_CMD_SET_DISPLAY_CLK_DIV = 0xD5;
  const uint8_t OLED_CMD_SET_PRECHARGE = 0xD9;
  const uint8_t OLED_CMD_SET_VCOMH_DESELCT = 0xDB;
  const uint8_t OLED_CMD_DEACTIVE_SCROLL = 0x2E;
  const uint8_t OLED_CMD_SET_PAGE_ADDR_MODE = 0x02;
  const uint8_t OLED_CMD_SET_HORI_ADDR_MODE = 0x00;

  /* Charge Pump (pg.62) */
  const uint8_t OLED_CMD_SET_CHARGE_PUMP = 0x8D;
};

#endif /* SSD1306_GUARD */
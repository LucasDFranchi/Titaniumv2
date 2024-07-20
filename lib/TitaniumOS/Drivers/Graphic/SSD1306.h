#include "Drivers/DriverInterface/IGraphicDriver.h"
#include "Kernel/error/error_enum.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#ifndef SSD1306_GUARD
#define SSD1306_GUARD

/**
 * @brief SSD1306 is a class that represents a driver for the SSD1306 OLED display.
 * 
 * This class provides methods to initialize and interact with the SSD1306 OLED display.
 * It inherits from the IGraphicDriver interface.
 */
class SSD1306 : public IGraphicDriver
{
public:
  /**
   * @brief Initializes the SSD1306 display.
   * 
   * @returns ESP_OK if initialization is successful, otherwise an error code.
   */
  titan_err_t Initialize(void);

  /**
   * @brief Processes the given memory area data and prints the pattern on the SSD1306 display.
   * 
   * @param memory_area_data The data representing the memory area.
   */
  void Process(uint8_t *memory_area_data);

private:
  i2c_config_t i2c_config; ///< I2C configuration structure.

  /* Private functions  */
private:
  /**
   * @brief Sends the initialization commands to the SSD1306 OLED display.
   * 
   * @returns ESP_OK if the initialization commands were sent successfully, otherwise an error code.
   */
  titan_err_t SendInitializationCommands_(void);

  /**
   * @brief Clears the display of the SSD1306 OLED module.
   * 
   * @returns ESP_OK if the display was cleared successfully, otherwise an error code.
   */
  titan_err_t ClearDisplay_(void);

  /**
   * @brief Prints a pattern on the SSD1306 OLED display.
   * 
   * @param pattern A pointer to the pattern data.
   * @returns ESP_OK if the pattern was printed successfully, Error::UNKNOW_FAIL if the pattern pointer is null.
   */
  titan_err_t PrintPattern_(uint8_t *pattern);

  /**
   * @brief Sets up the SSD1306 display for a specific page.
   * 
   * @param page The page number to set up (must be between 0 and 7).
   * @returns ESP_OK if the setup is successful, otherwise an error code.
   */
  titan_err_t SetupPage_(int page);

  /**
   * @brief Sends a reset command to the SSD1306 display.
   * 
   * This function resets the display by setting the OLED_I2C_RESET_GPIO pin to low for a brief period of time, and then setting it back to high.
   */
  void SendResetCommand_(void);

  /* Private constants */
private:
  const gpio_num_t OLED_I2C_MASTER_SDA_IO = GPIO_NUM_4; ///< GPIO number for I2C SDA.
  const gpio_num_t OLED_I2C_MASTER_SCL_IO = GPIO_NUM_15; ///< GPIO number for I2C SCL.
  const gpio_num_t OLED_I2C_RESET_GPIO = GPIO_NUM_16; ///< GPIO number for OLED reset.
  const int OLED_I2C_MASTER_FREQ_HZ = 400000; ///< I2C master clock frequency.

  const uint8_t OLED_I2C_ADDRESS = 0x3C; ///< I2C address of the SSD1306 OLED display.

  /* Control byte */
  const uint8_t OLED_CONTROL_BYTE_CMD_SINGLE = 0x80; ///< Control byte for single command.
  const uint8_t OLED_CONTROL_BYTE_CMD_STREAM = 0x00; ///< Control byte for command stream.
  const uint8_t OLED_CONTROL_BYTE_DATA_STREAM = 0x40; ///< Control byte for data stream.

  /* Fundamental commands (pg.28) */
  const uint8_t OLED_CMD_SET_CONTRAST = 0x81; ///< Command to set contrast.
  const uint8_t OLED_CMD_DISPLAY_RAM = 0xA4; ///< Command to display RAM content.
  const uint8_t OLED_CMD_DISPLAY_ALLON = 0xA5; ///< Command to display all pixels on.
  const uint8_t OLED_CMD_DISPLAY_NORMAL = 0xA6; ///< Command to set normal display mode.
  const uint8_t OLED_CMD_DISPLAY_INVERTED = 0xA7; ///< Command to set inverted display mode.
  const uint8_t OLED_CMD_DISPLAY_OFF = 0xAE; ///< Command to turn off display.
  const uint8_t OLED_CMD_DISPLAY_ON = 0xAF; ///< Command to turn on display.

  /* Addressing Command Table (pg.30) */
  const uint8_t OLED_CMD_SET_MEMORY_ADDR_MODE = 0x20; ///< Command to set memory addressing mode.
  const uint8_t OLED_CMD_SET_COLUMN_RANGE = 0x21; ///< Command to set column range.
  const uint8_t OLED_CMD_SET_PAGE_RANGE = 0x22; ///< Command to set page range.

  /* Hardware Config (pg.31) */
  const uint8_t OLED_CMD_SET_DISPLAY_START_LINE = 0x40; ///< Command to set display start line.
  const uint8_t OLED_CMD_SET_SEGMENT_REMAP = 0xA1; ///< Command to set segment remap.
  const uint8_t OLED_CMD_SET_MUX_RATIO = 0xA8; ///< Command to set multiplex ratio.
  const uint8_t OLED_CMD_SET_COM_SCAN_MODE = 0xC8; ///< Command to set COM scan mode.
  const uint8_t OLED_CMD_SET_DISPLAY_OFFSET = 0xD3; ///< Command to set display offset.
  const uint8_t OLED_CMD_SET_COM_PIN_MAP = 0xDA; ///< Command to set COM pin map.
  const uint8_t OLED_CMD_NOP = 0xE3; ///< No operation command.

  /* Timing and Driving Scheme (pg.32) */
  const uint8_t OLED_CMD_SET_DISPLAY_CLK_DIV = 0xD5; ///< Command to set display clock divide ratio/oscillator frequency.
  const uint8_t OLED_CMD_SET_PRECHARGE = 0xD9; ///< Command to set pre-charge period.
  const uint8_t OLED_CMD_SET_VCOMH_DESELCT = 0xDB; ///< Command to set Vcomh deselect level.
  const uint8_t OLED_CMD_DEACTIVE_SCROLL = 0x2E; ///< Command to deactivate scroll.
  const uint8_t OLED_CMD_SET_PAGE_ADDR_MODE = 0x02; ///< Command to set page addressing mode.
  const uint8_t OLED_CMD_SET_HORI_ADDR_MODE = 0x00; ///< Command to set horizontal addressing mode.

  /* Charge Pump (pg.62) */
  const uint8_t OLED_CMD_SET_CHARGE_PUMP = 0x8D; ///< Command to enable or disable charge pump.
};

#endif /* SSD1306_GUARD */

#include "SSD1306.h"

#include "driver/gpio.h"
#include "string.h"

/**
 * @brief
 *
 * @return esp_err_t
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Driver installation error
 */
esp_err_t SSD1306::Init(void) {
  esp_err_t ret = ESP_OK;

  memset(&this->i2c_config, 0, sizeof(i2c_config_t));

  this->i2c_config.mode = I2C_MODE_MASTER;
  this->i2c_config.sda_io_num = this->OLED_I2C_MASTER_SDA_IO;
  this->i2c_config.scl_io_num = this->OLED_I2C_MASTER_SCL_IO;
  this->i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
  this->i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
  this->i2c_config.master.clk_speed = this->OLED_I2C_MASTER_FREQ_HZ;
  this->i2c_config.clk_flags = 0;

  ret += i2c_param_config(I2C_NUM_0, &i2c_config);
  ret += i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
  ret += this->SendInitializationCommands_();
  ret += this->ClearDisplay_();

  return ret;
}

void SSD1306::UpdatePage(void) {
    for (uint8_t i= 0; i < 0xFF; i++){
        this->PrintPattern_(i);
    }
  // https://embeddedexplorer.com/esp32-i2c-tutorial/
  // https://github.com/yanbe/ssd1306-esp-idf-i2c/blob/master/main/main.c
}

/**
 * @brief
 *
 * @return esp_err_t
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM The static buffer used to create `cmd_handler` is too
 * small
 *     - ESP_FAIL Sending command error, slave hasn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t SSD1306::SendInitializationCommands_(void) {
  esp_err_t ret = ESP_OK;

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();

  ret += i2c_master_start(cmd);
  ret += i2c_master_write_byte(
      cmd, (this->OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
  ret += i2c_master_write_byte(cmd, this->OLED_CONTROL_BYTE_CMD_STREAM, true);

  ret += i2c_master_write_byte(cmd, this->OLED_CMD_SET_CHARGE_PUMP, true);
  ret += i2c_master_write_byte(cmd, 0x14, true);

  ret += i2c_master_write_byte(cmd, this->OLED_CMD_SET_SEGMENT_REMAP, true);
  ret += i2c_master_write_byte(cmd, this->OLED_CMD_SET_COM_SCAN_MODE, true);

  ret += i2c_master_write_byte(cmd, this->OLED_CMD_DISPLAY_ON, true);
  ret += i2c_master_stop(cmd);

  ret += i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);

  i2c_cmd_link_delete(cmd);

  return ret;
}

/**
 * @brief
 *
 * @return esp_err_t
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM The static buffer used to create `cmd_handler` is too
 * small
 *     - ESP_FAIL Sending command error, slave hasn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t SSD1306::PrintPattern_(uint8_t pattern) {
  i2c_cmd_handle_t cmd;
  esp_err_t ret = ESP_OK;
  uint8_t zero[128] = {pattern};

  for (uint8_t i = 0; i < 8; i++) {
    cmd = i2c_cmd_link_create();
    ret += i2c_master_start(cmd);
    ret += i2c_master_write_byte(
        cmd, (this->OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    ret += i2c_master_write_byte(cmd, this->OLED_CONTROL_BYTE_CMD_SINGLE, true);
    ret += i2c_master_write_byte(cmd, 0xB0 | i, true);

    ret +=
        i2c_master_write_byte(cmd, this->OLED_CONTROL_BYTE_DATA_STREAM, true);
    ret += i2c_master_write(cmd, zero, 128, true);
    ret += i2c_master_stop(cmd);
    ret += i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
  }

  return ret;
}

/**
 * @brief
 *
 * @return esp_err_t
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM The static buffer used to create `cmd_handler` is too
 * small
 *     - ESP_FAIL Sending command error, slave hasn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t SSD1306::ClearDisplay_(void) { return this->PrintPattern_(0); }
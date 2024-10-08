#include "SSD1306.h"

#include "stdio.h"
#include "string.h"

/**
 * @brief Initializes the SSD1306 display.
 *
 * This function sets up the I2C configuration and sends the initialization commands to the SSD1306 display.
 *
 * @returns ESP_OK if initialization is successful, otherwise an error code.
 */
titan_err_t SSD1306::Initialize(void) {
    titan_err_t ret = ESP_OK;

    memset(&this->i2c_config, 0, sizeof(i2c_config_t));

    this->i2c_config.mode = I2C_MODE_MASTER;
    this->i2c_config.sda_io_num = this->OLED_I2C_MASTER_SDA_IO;
    this->i2c_config.scl_io_num = this->OLED_I2C_MASTER_SCL_IO;
    this->i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    this->i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    this->i2c_config.master.clk_speed = this->OLED_I2C_MASTER_FREQ_HZ;
    this->i2c_config.clk_flags = 0;

    this->SendResetCommand_();

    ret += i2c_param_config(I2C_NUM_0, &i2c_config);
    ret += i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
    ret += this->SendInitializationCommands_();
    // ret += this->ClearDisplay_();

    return ret;
}

/**
 * @brief Processes the given memory area data and prints the pattern on the SSD1306 display.
 *
 * @param memory_area_data The data representing the memory area.
 */
void SSD1306::Process(uint8_t* memory_area_data) {
    this->PrintPattern_(memory_area_data);
}

/**
 * @brief Sends the initialization commands to the SSD1306 OLED display.
 *
 * This function sends a series of commands to the SSD1306 to initialize the display.
 *
 * @returns ESP_OK if the initialization commands were sent successfully, otherwise an error code.
 */
titan_err_t SSD1306::SendInitializationCommands_(void) {
    titan_err_t ret = ESP_OK;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    ret += i2c_master_start(cmd);
    ret += i2c_master_write_byte(
        cmd, (this->OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    ret += i2c_master_write_byte(cmd, this->OLED_CONTROL_BYTE_CMD_STREAM, true);

    ret += i2c_master_write_byte(cmd, this->OLED_CMD_SET_CHARGE_PUMP, true);
    ret += i2c_master_write_byte(cmd, 0x14, true);

    ret += i2c_master_write_byte(cmd, this->OLED_CMD_SET_SEGMENT_REMAP, true); // reverse left-right mapping
    ret += i2c_master_write_byte(cmd, this->OLED_CMD_SET_COM_SCAN_MODE, true); // reverse up-bottom mapping

    ret += i2c_master_write_byte(cmd, this->OLED_CMD_DISPLAY_ON, true);
    ret += i2c_master_stop(cmd);

    ret += i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);

    i2c_cmd_link_delete(cmd);

    return ret;
}

/**
 * @brief Prints a pattern on the SSD1306 OLED display.
 *
 * This function sends a pattern of data to the SSD1306 display to be rendered.
 *
 * @param pattern A pointer to the pattern data.
 * @returns ESP_OK if the pattern was printed successfully, Error::UNKNOW_FAIL if the pattern pointer is null.
 */
titan_err_t SSD1306::PrintPattern_(uint8_t* pattern) {
    i2c_cmd_handle_t cmd;
    titan_err_t ret = ESP_OK;

    if (pattern == nullptr) {
        return Error::UNKNOW_FAIL;
    }

    for (uint8_t i = 0; i < 8; i++) {
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(
            cmd, (this->OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, this->OLED_CONTROL_BYTE_CMD_SINGLE, true);
        i2c_master_write_byte(cmd, 0xB0 | i, true);
        i2c_master_write_byte(cmd, this->OLED_CONTROL_BYTE_DATA_STREAM, true);
        i2c_master_write(cmd, &pattern[128 * i], 128, true);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
    }

    return ret;
}

/**
 * @brief Clears the display of the SSD1306 OLED module.
 *
 * This function clears the display by printing a pattern of zeros.
 *
 * @returns ESP_OK if the display was cleared successfully, otherwise an error code.
 */
titan_err_t SSD1306::ClearDisplay_(void) {
    return this->PrintPattern_(0);
}

/**
 * @brief Sends a reset command to the SSD1306 display.
 *
 * This function resets the display by setting the OLED_I2C_RESET_GPIO pin to low for a brief period of time, and then setting it back to high.
 */
void SSD1306::SendResetCommand_(void) {
    gpio_reset_pin(this->OLED_I2C_RESET_GPIO);
    gpio_set_direction(this->OLED_I2C_RESET_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(this->OLED_I2C_RESET_GPIO, 0);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    gpio_set_level(this->OLED_I2C_RESET_GPIO, 1);
}

/**
 * @brief Sets up the SSD1306 display for a specific page.
 *
 * This function configures the display to start writing data at a specific page.
 *
 * @param page The page number to set up (must be between 0 and 7).
 * @returns ESP_OK if the setup is successful, otherwise an error code.
 */
titan_err_t SSD1306::SetupPage_(int page) {
    i2c_cmd_handle_t cmd;
    titan_err_t ret = ESP_OK;

    if ((page > 7) || (page < 0)) {
        ret = ESP_ERR_INVALID_ARG;
    } else {
        cmd = i2c_cmd_link_create();
        ret += i2c_master_start(cmd);
        ret += i2c_master_write_byte(
            cmd, (this->OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

        ret += i2c_master_write_byte(cmd, this->OLED_CONTROL_BYTE_CMD_STREAM, true);
        ret += i2c_master_write_byte(cmd, (0x00), true);
        ret += i2c_master_write_byte(cmd, (0x10), true);
        ret += i2c_master_write_byte(cmd, 0xB0 | page, true);

        ret += i2c_master_stop(cmd);
        ret += i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);

        i2c_cmd_link_delete(cmd);
    }

    return ret;
}

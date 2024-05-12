#ifndef UART_DRIVER_GUARD
#define UART_DRIVER_GUARDW

#include "Drivers/DriverInterface/ICommunicationDriver.h"

#include "driver/uart.h"

namespace Baudrate {
    constexpr uint32_t BaudRate9600   = 9600;
    constexpr uint32_t BaudRate19200  = 19200;
    constexpr uint32_t BaudRate38400  = 38400;
    constexpr uint32_t BaudRate57600  = 57600;
    constexpr uint32_t BaudRate115200 = 115200;
    constexpr uint32_t BaudRate230400 = 230400;
}  // namespace Baudrate

/**
 * @class UARTDriver
 * @brief A class for UART communication implementing IDriverInterface.
 */
class UARTDriver : public IDriverInterface {
   public:
    /**
     * @brief Default constructor for UARTDriver.
     */
    UARTDriver(uart_port_t uart_num, uint32_t baud_rate, uint16_t buffer_size)
        : _uart_num(uart_num), _baud_rate(baud_rate), _buffer_size(buffer_size) {
        auto result = ESP_OK;

        this->_uart_config.baud_rate           = this->_baud_rate;
        this->_uart_config.data_bits           = UART_DATA_8_BITS;
        this->_uart_config.parity              = UART_PARITY_DISABLE;
        this->_uart_config.stop_bits           = UART_STOP_BITS_1;
        this->_uart_config.flow_ctrl           = UART_HW_FLOWCTRL_DISABLE;
        this->_uart_config.rx_flow_ctrl_thresh = 0;
        this->_uart_config.source_clk          = UART_SCLK_APB;

        result += uart_param_config(this->_uart_num, &this->_uart_config);
        result += uart_set_pin(this->_uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                               UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        result +=
            uart_driver_install(this->_uart_num, this->_buffer_size, 0, 0, NULL, 0);

        uart_flush(this->_uart_num);

        if (result == ESP_OK) {
            this->_is_initialized = true;
        }
    }

    virtual ~UARTDriver() {}

    /**
     * @brief Reads data from the UART driver.
     * @param[in] raw_bytes A unique pointer to an array where the read bytes will be stored.
     * @return The number of bytes read.
     */
    uint16_t Read(uint8_t* raw_bytes) override {
        auto result = ESP_FAIL;

        do {
            if (raw_bytes == nullptr) {
                break;
            }

            result = uart_read_bytes(this->_uart_num, raw_bytes, this->_buffer_size, pdMS_TO_TICKS(1000));

        } while (0);

        return result;
    }

    /**
     * @brief Writes data to the UART driver.
     * @param[in] raw_bytes A unique pointer to an array containing the bytes to write.
     * @param[in] size The number of bytes to write.
     * @return ESP_OK on success, or an error code from esp_err_t on failure.
     */
    esp_err_t Write(uint8_t* raw_bytes, uint16_t size) override {
        auto result = ESP_FAIL;

        do {
            if (raw_bytes == nullptr) {
                break;
            }

            uint16_t written_bytes = uart_write_bytes(this->_uart_num, raw_bytes, size);
            result                 = written_bytes == size ? ESP_OK : ESP_FAIL;

        } while (0);

        uart_flush(this->_uart_num);

        return result;
    }

    uint16_t buffer_size() const {
        return this->_buffer_size;
    }

   private:
    uart_config_t _uart_config{0};
    uart_port_t _uart_num;
    uint32_t _baud_rate;
    uint16_t _buffer_size;
    bool _is_initialized = false;
};

#endif /* UART_DRIVER_GUARD */
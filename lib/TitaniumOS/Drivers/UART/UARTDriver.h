#ifndef UART_DRIVER_GUARD
#define UART_DRIVER_GUARD

#include "Drivers/DriverInterface/ICommunicationDriver.h"
#include "Application/error/error_enum.h"
#include "driver/uart.h"

namespace Baudrate {
    constexpr uint32_t BaudRate9600   = 9600;   /**< Baud rate of 9600. */
    constexpr uint32_t BaudRate19200  = 19200;  /**< Baud rate of 19200. */
    constexpr uint32_t BaudRate38400  = 38400;  /**< Baud rate of 38400. */
    constexpr uint32_t BaudRate57600  = 57600;  /**< Baud rate of 57600. */
    constexpr uint32_t BaudRate115200 = 115200; /**< Baud rate of 115200. */
    constexpr uint32_t BaudRate230400 = 230400; /**< Baud rate of 230400. */
}  // namespace Baudrate

/**
 * @class UARTDriver
 * @brief A class for UART communication implementing IDriverInterface.
 */
class UARTDriver : public IDriverInterface {
   public:
    /**
     * @brief Constructor for UARTDriver.
     *
     * Initializes UART with specified parameters.
     *
     * @param[in] uart_num UART port number to initialize.
     * @param[in] baud_rate Baud rate for UART communication.
     * @param[in] buffer_size Size of the buffer for UART operations.
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
            uart_driver_install(this->_uart_num, this->_buffer_size, 0, 0, nullptr, 0);

        uart_flush(this->_uart_num);

        if (result == ESP_OK) {
            this->_is_initialized = true;
        }
    }

    virtual ~UARTDriver() {} /**< Destructor for UARTDriver. */

    /**
     * @brief Reads data from the UART driver.
     *
     * @param[out] raw_bytes Pointer to an array where the read bytes will be stored.
     * @return The number of bytes read, or Error::UNKNOW_FAIL if an error occurred.
     */
    uint16_t Read(uint8_t* raw_bytes) override {
        auto result = Error::UNKNOW_FAIL;

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
     *
     * @param[in] raw_bytes Pointer to an array containing the bytes to write.
     * @param[in] size Number of bytes to write.
     * @return ESP_OK on success, or an error code from titan_err_t on failure.
     */
    titan_err_t Write(uint8_t* raw_bytes, uint16_t size) override {
        auto result = Error::UNKNOW_FAIL;

        do {
            if (raw_bytes == nullptr) {
                break;
            }

            uint16_t written_bytes = uart_write_bytes(this->_uart_num, raw_bytes, size);
            result                 = (written_bytes == size) ? ESP_OK : Error::UNKNOW_FAIL;

        } while (0);

        uart_flush(this->_uart_num);

        return result;
    }

    /**
     * @brief Retrieves the buffer size used for UART operations.
     *
     * @return Size of the buffer.
     */
    uint16_t buffer_size() const {
        return this->_buffer_size;
    }

   private:
    uart_config_t _uart_config{0}; /**< UART configuration structure. */
    uart_port_t _uart_num;         /**< UART port number. */
    uint32_t _baud_rate;           /**< Baud rate for UART communication. */
    uint16_t _buffer_size;         /**< Size of the buffer for UART operations. */
    bool _is_initialized = false;  /**< Flag indicating if UART is initialized. */
};

#endif /* UART_DRIVER_GUARD */

#ifndef TEST_DRIVER_GUARD
#define TEST_DRIVER_GUARD

#include "Drivers/DriverInterface/ICommunicationDriver.h"

#include "string.h"

class TestDriver : public IDriverInterface {
   public:
    TestDriver(uint16_t buffer_size)
        : _buffer_size(buffer_size) {
    }

    virtual ~TestDriver() {} /**< Destructor for TestDriver. */

    uint16_t Read(uint8_t* raw_bytes) override {
        auto result = 0;

        do {
            if (raw_bytes == nullptr) {
                break;
            }
            if (this->written_bytes <= 0) {
                break;
            }

            memcpy(raw_bytes, this->buffer, this->written_bytes);
            result              = this->written_bytes;
            this->written_bytes = 0;
        } while (0);

        return result;
    }

    esp_err_t Write(uint8_t* raw_bytes, uint16_t size) override {
        auto result = ESP_FAIL;

        do {
            if (raw_bytes == nullptr) {
                break;
            }
            memset(this->buffer, 0, sizeof(this->buffer));
            memcpy(this->buffer, raw_bytes, size);
            this->written_bytes = size;
            result              = ESP_OK;

        } while (0);

        return result;
    }

    uint16_t buffer_size() const {
        return this->_buffer_size;
    }

    uint16_t _buffer_size; /**< Size of the buffer for UART operations. */
    uint8_t buffer[1024]   = {0};
    uint16_t written_bytes = 0;
    bool _is_initialized   = false; /**< Flag indicating if UART is initialized. */
};

#endif /* TEST_DRIVER_GUARD */
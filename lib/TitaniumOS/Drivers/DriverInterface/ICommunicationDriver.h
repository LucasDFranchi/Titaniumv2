#ifndef I_COMMUNICATION_DRIVER
#define I_COMMUNICATION_DRIVER

#include "esp_err.h"

#include <memory>

class IDriverInterface {
   public:
    virtual ~IDriverInterface() {}

    virtual esp_err_t Write(uint8_t* raw_bytes, uint16_t size) = 0;
    virtual uint16_t Read(uint8_t* raw_bytes)                  = 0;

    virtual uint16_t buffer_size() const = 0;
};

#endif  // I_COMMUNICATION_DRIVER

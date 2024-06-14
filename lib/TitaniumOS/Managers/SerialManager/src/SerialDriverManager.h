#ifndef SERIAL_DRIVER_MANAGER_GUARD
#define SERIAL_DRIVER_MANAGER_GUARD

#include "Process/ProcessTemplate.h"

#include "driver/uart.h"
#include "esp_err.h"

namespace Baudrate {
    constexpr uint32_t BaudRate9600   = 9600;
    constexpr uint32_t BaudRate19200  = 19200;
    constexpr uint32_t BaudRate38400  = 38400;
    constexpr uint32_t BaudRate57600  = 57600;
    constexpr uint32_t BaudRate115200 = 115200;
    constexpr uint32_t BaudRate230400 = 230400;
}  // namespace Baudrate

/**
 * A class that manages the graphic driver process.
 */
class SerialDriverManager : public ProcessTemplate {
   public:
    SerialDriverManager(const char* name, uint32_t memory, UBaseType_t priority)
        : ProcessTemplate(this, name, memory, priority, &this->process_handler){};
    void Execute(void);

   private:
    esp_err_t Initialize(void);

   private:
    uart_config_t uart_config{0};
    uint8_t buffer[2048]         = {0};
    TaskHandle_t process_handler = NULL;
};

#endif /* SERIAL_DRIVER_MANAGER_GUARD */
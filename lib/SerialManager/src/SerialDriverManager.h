#include "ProcessTemplate.h"

#include "driver/uart.h"
#include "esp_err.h"

#ifndef SERIAL_DRIVER_MANAGER_GUARD
#define SERIAL_DRIVER_MANAGER_GUARD

/**
 * A class that manages the graphic driver process.
 */
class SerialDriverManager : public ProcessTemplate{
    public:
    SerialDriverManager() : ProcessTemplate(this, "Serial Proccess", 10240, 1, &this->process_handler) { };
    void Execute(void);

    private:
    esp_err_t Initialize_(void);

    private:
    uart_config_t uart_config;
    const uint32_t baud_rate_ = 115200;
    const uint32_t buffer_size_ = 1024;
    uint8_t input_buffer_[1024] = {0};
    TaskHandle_t process_handler = NULL;
};

#endif /* SERIAL_DRIVER_MANAGER_GUARD */
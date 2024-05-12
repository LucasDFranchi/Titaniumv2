#ifndef SERIAL_DRIVER_MANAGER_GUARD
#define SERIAL_DRIVER_MANAGER_GUARD

// #include "ProcessTemplate.h"

// #include "driver/uart.h"
// #include "esp_err.h"

// /**
//  * A class that manages the graphic driver process.
//  */
// class SerialDriverManager : public ProcessTemplate{
//     public:
//     SerialDriverManager(const char* name, uint32_t memory, UBaseType_t  priority) : ProcessTemplate(this, name, memory, priority, &this->process_handler_) { };
//     void Execute(void);

//     private:
//     esp_err_t Initialize_(void);

//     private:
//     uart_config_t uart_config;
//     const uint32_t baud_rate_ = 115200;
//     // const uint32_t baud_rate_ = 230400;
//     const uint32_t buffer_size_ = 2048;
//     uint8_t input_buffer_[2048] = {0};
//     TaskHandle_t process_handler_ = NULL;
// };

#endif /* SERIAL_DRIVER_MANAGER_GUARD */
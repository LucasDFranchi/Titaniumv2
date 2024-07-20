#ifndef KERNEL_H
#define KERNEL_H

#include "nvs_flash.h"

#include "Application/error/error_enum.h"
#include "Protocols/Protobuf/inc/titanium.pb.h"
#include <Drivers/LoRa/LoRaDriver.h>
#include <Drivers/UART/UARTDriver.h>
#include <HAL/gpio/GPIOManager.h>
#include <HAL/memory/SharedMemoryManager.h>
#include <HAL/spi/SPIManager.h>
#include <SystemProcess/CommunicationProcess/inc/CommunicationProcess.h>
#include <SystemProcess/HTTPServerProcess/inc/HTTPServerProcess.h>
#include <SystemProcess/MQTTClientProcess/inc/MQTTClientProcess.h>
#include <SystemProcess/NetworkProcess/inc/NetworkProcess.h>

class Application {
   public:
    /**
     * @brief Constructor for Application class.
     *        Initializes NVS flash and HAL components.
     */
    Application() {
        nvs_flash_erase();
        ESP_ERROR_CHECK(nvs_flash_init());

        this->InitializeHAL();
    };
    titan_err_t EnableNetworkProcess(uint32_t process_stack, uint8_t process_priority, bool can_fail = false);
    titan_err_t EnableHTTPServerProcess(uint32_t process_stack, uint8_t process_priority, bool can_fail = false);
    titan_err_t EnableUartProcess(uint32_t process_stack, uint8_t process_priority, bool can_fail = false);
    titan_err_t EnableLoraProcess(uint32_t process_stack, uint8_t process_priority, bool can_fail = false);
    titan_err_t EnableMQTTClientProcess(uint32_t process_stack, uint8_t process_priority, bool can_fail = false);
    titan_err_t SignUpSharedArea(uint8_t index, uint16_t size_in_bytes,
                                 AccessType access_type, bool can_fail = false);

    void InjectDebugCredentials(const char* ssid, const char* password);

    /**
     * @brief Injects debug data into a specific memory area.
     *        This function is intended for debugging purposes and should not be exposed in production.
     */
    template <typename T>
    void InjectMemoryAreaData(uint8_t area_index, T& protobuf, const pb_msgdesc_t msg_desc) {
        ESP_LOGI("Application Debug", "Start Writing Injection!");
        auto ret = this->_shared_memory_manager->Write(area_index, protobuf, msg_desc);

        if (ret != Error::NO_ERROR) {
            ESP_LOGE("Application Debug", "Was not possible to inject memory data in area %d, returned %d", area_index, ret);
        }
    }

   private:
    void InitializeHAL(void);

   private:
    bool _spi_initialized  = false;                               ///< Flag indicating SPI initialization status.
    bool _gpio_initialized = false;                               ///< Flag indicating GPIO initialization status.
    SharedMemoryManager* _shared_memory_manager;                  ///< Pointer to SharedMemoryManager instance.
    GPIOManager* _gpio_manager;                                   ///< Pointer to GPIOManager instance.
    SPIManager* _spi_manager;                                     ///< Pointer to SPIManager instance.
    NetworkProcess* _network_process;                             ///< Pointer to NetworkManager instance.
    HTTPServerProcess* _http_server_process;                      ///< Pointer to HTTPServerManager instance.
    CommunicationProcess* _uart_communication_process = nullptr;  ///< Pointer to UART Communication Manager instance.
    CommunicationProcess* _lora_communication_process = nullptr;  ///< Pointer to LoRa Communication Manager instance.
    MQTTClientProcess* _mqtt_client_process           = nullptr;  ///<
};

#endif /* KERNEL_H */
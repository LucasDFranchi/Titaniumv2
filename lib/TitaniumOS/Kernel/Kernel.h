#ifndef KERNEL_H
#define KERNEL_H

#include "nvs_flash.h"

#include <Drivers/LoRa/LoRaDriver.h>
#include <Drivers/UART/UARTDriver.h>
#include <HAL/gpio/GPIOManager.h>
#include <HAL/memory/SharedMemoryManager.h>
#include <HAL/spi/SPIManager.h>
#include <Managers/HTTPServerManager/inc/HTTPServerManager.h>
#include <Managers/ManagersAreaIndex.h>
#include <Managers/NetworkManager/inc/NetworkManager.h>
#include <Managers/SerialCommunicationManager/inc/SerialCommunicationManager.h>

class Kernel {
   public:
    /**
     * @brief Constructor for Kernel class.
     *        Initializes NVS flash and HAL components.
     */
    Kernel() {
        nvs_flash_erase();
        ESP_ERROR_CHECK(nvs_flash_init());

        this->InitializeHAL();
    };
    esp_err_t InitializeNetwork(uint32_t process_stack, uint8_t process_priority, bool can_fail = false);
    esp_err_t InitializeHTTPServer(uint32_t process_stack, uint8_t process_priority, bool can_fail = false);
    esp_err_t InitializeLora(uint32_t process_stack, uint8_t process_priority, bool can_fail = false);
    esp_err_t InitializeUart(uint32_t process_stack, uint8_t process_priority, bool can_fail = false);
    esp_err_t SignUpSharedArea(uint8_t index, uint16_t size_in_bytes,
                               AccessType access_type, bool can_fail = false);

    void InjectDebugCredentials(const char * ssid, const char* password);

   private:
    void InitializeHAL(void);

   private:
    bool _spi_initialized  = false;               ///< Flag indicating SPI initialization status.
    bool _gpio_initialized = false;               ///< Flag indicating GPIO initialization status.
    SharedMemoryManager* _shared_memory_manager;  ///< Pointer to SharedMemoryManager instance.
    GPIOManager* _gpio_manager;                   ///< Pointer to GPIOManager instance.
    SPIManager* _spi_manager;                     ///< Pointer to SPIManager instance.
    NetworkManager* _network_manager;             ///< Pointer to NetworkManager instance.
    HTTPServerManager* _http_server_manager;      ///< Pointer to HTTPServerManager instance.
    // TODO: Change SerialCommunciation Manager to CommunicationManager
    SerialCommunicationManager* _uart_communication_manager = nullptr; ///< Pointer to UART Communication Manager instance.
    SerialCommunicationManager* _lora_communication_manager = nullptr; ///< Pointer to LoRa Communication Manager instance.
};

#endif /* KERNEL_H */
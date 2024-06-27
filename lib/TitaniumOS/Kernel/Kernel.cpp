/**
 * @file Kernel.cpp
 * @brief Implementation of the Kernel class methods.
 */

#include "Kernel.h"

// Initialize static member variables
SharedMemoryManager* SharedMemoryManager::singleton_pointer_ = nullptr;
GPIOManager* GPIOManager::singleton_pointer_                 = nullptr;
SPIManager* SPIManager::singleton_pointer_                   = nullptr;

/**
 * @brief Initializes the Hardware Abstraction Layer (HAL) components.
 *        Initializes shared memory manager, GPIO manager, and SPI manager.
 */
void Kernel::InitializeHAL(void) {
    this->_shared_memory_manager = SharedMemoryManager::GetInstance();
    this->_gpio_manager          = GPIOManager::GetInstance();
    this->_spi_manager           = SPIManager::GetInstance();

    ESP_ERROR_CHECK(this->_shared_memory_manager->Initialize());

    if (this->_gpio_manager->Initialize() == ESP_OK) {
        this->_gpio_initialized = true;
    }
    if (this->_spi_manager->Initialize(GPIO_NUM_27, GPIO_NUM_19, GPIO_NUM_5) == ESP_OK) {
        this->_spi_initialized = true;
    }

    ESP_ERROR_CHECK(this->_shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::SCRATCH, 128, READ_WRITE));
}

/**
 * @brief Initializes the network component.
 * @param[in] process_stack Stack size for the network process.
 * @param[in] process_priority Priority of the network process.
 * @param[in] can_fail Flag indicating if initialization failure should be tolerated.
 * @return ESP_OK if initialization succeeds, otherwise an error code.
 */
esp_err_t Kernel::InitializeNetwork(uint32_t process_stack, uint8_t process_priority, bool can_fail) {
    auto result            = ESP_OK;
    this->_network_manager = new NetworkManager("Network Proccess", process_stack, process_priority);  // 10240 4
    this->_network_manager->InitializeProcess();

    result += this->_shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::CREDENTIALS, sizeof(credentials_st), READ_WRITE);
    result += this->_shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::CONNECTION, sizeof(connection_st), READ_WRITE);
    if (!can_fail) {
        ESP_ERROR_CHECK(result);
    }

    return result;
}

/**
 * @brief Initializes the HTTP server component.
 * @param[in] process_stack Stack size for the HTTP server process.
 * @param[in] process_priority Priority of the HTTP server process.
 * @param[in] can_fail Flag indicating if initialization failure should be tolerated.
 * @return ESP_OK if initialization succeeds, otherwise an error code.
 */
esp_err_t Kernel::InitializeHTTPServer(uint32_t process_stack, uint8_t process_priority, bool can_fail) {
    auto result                = ESP_OK;
    this->_http_server_manager = new HTTPServerManager("HTTP Server Process", process_stack, process_priority);
    this->_http_server_manager->InitializeProcess();

    if (!can_fail) {
        ESP_ERROR_CHECK(result);
    }

    return result;
}

/**
 * @brief Initializes the UART communication component.
 * @param[in] process_priority Priority of the UART communication process.
 * @param[in] process_stack Stack size for the UART communication process.
 * @param[in] can_fail Flag indicating if initialization failure should be tolerated.
 * @return ESP_OK if initialization succeeds, otherwise an error code.
 */
esp_err_t Kernel::InitializeUart(uint32_t process_stack, uint8_t process_priority, bool can_fail) {
    auto result                       = ESP_OK;
    this->_uart_communication_manager = new SerialCommunicationManager("UART Communication Proccess", process_stack, process_priority);
    this->_uart_communication_manager->InstallDriver(
        new UARTDriver(UART_NUM_0, Baudrate::BaudRate115200, 1024),
        ManagersAreaIndex::UART_RECEIVE,
        ManagersAreaIndex::UART_TRANSMIT);

    result += this->_shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::UART_RECEIVE, 256, READ_WRITE);
    result += this->_shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::UART_TRANSMIT, 256, READ_WRITE);

    this->_uart_communication_manager->InitializeProcess();

    if (!can_fail) {
        ESP_ERROR_CHECK(result);
    }

    return result;
}

/**
 * @brief Initializes the LoRa communication component.
 * @param[in] process_stack Stack size for the LoRa communication process.
 * @param[in] process_priority Priority of the LoRa communication process.
 * @param[in] can_fail Flag indicating if initialization failure should be tolerated.
 * @return ESP_OK if initialization succeeds, otherwise an error code.
 */
esp_err_t Kernel::InitializeLora(uint32_t process_stack, uint8_t process_priority, bool can_fail) {
    auto result = ESP_OK;

    do {
        if ((!this->_spi_initialized) || (!this->_gpio_initialized)) {
            result = ESP_FAIL;
            break;
        }

        this->_lora_communication_manager = new SerialCommunicationManager("LoRa Communication Proccess", process_stack, process_priority);
        this->_lora_communication_manager->InstallDriver(
            new LoRaDriver(Regions::BRAZIL, CRCMode::DISABLE, 255),
            ManagersAreaIndex::LORA_RECEIVE,
            ManagersAreaIndex::LORA_TRANSMIT);

        result += this->_shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::LORA_RECEIVE, 256, READ_WRITE);
        result += this->_shared_memory_manager->SignUpSharedArea(ManagersAreaIndex::LORA_TRANSMIT, 256, READ_WRITE);

        this->_lora_communication_manager->InitializeProcess();

    } while (0);

    if (!can_fail) {
        ESP_ERROR_CHECK(result);
    }

    return result;
}

/**
 * @brief Signs up a shared memory area.
 * @param[in] index Index of the shared memory area.
 * @param[in] size_in_bytes Size of the shared memory area in bytes.
 * @param[in] access_type Access type for the shared memory area.
 * @param[in] can_fail Flag indicating if sign-up failure should be tolerated.
 * @return ESP_OK if sign-up succeeds, otherwise an error code.
 */
esp_err_t Kernel::SignUpSharedArea(uint8_t index, uint16_t size_in_bytes, AccessType access_type, bool can_fail) {
    auto result = this->_shared_memory_manager->SignUpSharedArea(index, size_in_bytes, access_type);

    if (!can_fail) {
        ESP_ERROR_CHECK(result);
    }

    return result;
}

/**
 * @brief Injects debug credentials into the credentials shared memory area.
 *        This function is intended for debugging purposes and should not be exposed in production.
 */
void Kernel::InjectDebugCredentials(const char * ssid, const char* password) {
     credentials_st credentials_debug;

    memcpy_s<uint8_t>(credentials_debug.sta_ssid, (uint8_t*)ssid, sizeof(credentials_debug.sta_ssid));
    memcpy_s<uint8_t>(credentials_debug.sta_password, (uint8_t*)password, sizeof(credentials_debug.sta_password));

    this->_shared_memory_manager->Write(ManagersAreaIndex::CREDENTIALS, sizeof(credentials_st), &credentials_debug);
}
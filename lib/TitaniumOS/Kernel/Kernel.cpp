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

    // ESP_ERROR_CHECK(this->_shared_memory_manager->SignUpSharedArea(ProtobufIndex::SCRATCH, 128, READ_WRITE));
}

/**
 * @brief Initializes the network component.
 * @param[in] process_stack Stack size for the network process.
 * @param[in] process_priority Priority of the network process.
 * @param[in] can_fail Flag indicating if initialization failure should be tolerated.
 * @return ESP_OK if initialization succeeds, otherwise an error code.
 */
esp_err_t Kernel::EnableNetworkProcess(uint32_t process_stack, uint8_t process_priority, bool can_fail) {
    auto result            = ESP_OK;
    this->_network_process = new NetworkProcess("Network Proccess", process_stack, process_priority);  // 10240 4

    result += this->_shared_memory_manager->SignUpSharedArea(ProtobufIndex::CREDENTIALS, CredentialsProtobuf::GetStaticMaxSize(), READ_WRITE);
    result += this->_shared_memory_manager->SignUpSharedArea(ProtobufIndex::CONNECTION, ConnectionStatusProtobuf::GetStaticMaxSize(), READ_WRITE);
    if (!can_fail) {
        ESP_ERROR_CHECK(result);
    }

    this->_network_process->InitializeProcess();

    return result;
}

/**
 * @brief Initializes the HTTP server component.
 * @param[in] process_stack Stack size for the HTTP server process.
 * @param[in] process_priority Priority of the HTTP server process.
 * @param[in] can_fail Flag indicating if initialization failure should be tolerated.
 * @return ESP_OK if initialization succeeds, otherwise an error code.
 */
esp_err_t Kernel::EnableHTTPServerProcess(uint32_t process_stack, uint8_t process_priority, bool can_fail) {
    auto result                = ESP_OK;
    this->_http_server_process = new HTTPServerProcess("HTTP Server Process", process_stack, process_priority);
    this->_http_server_process->InitializeProcess();

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
esp_err_t Kernel::EnableUartProcess(uint32_t process_stack, uint8_t process_priority, bool can_fail) {
    auto result                       = ESP_OK;
    this->_uart_communication_process = new CommunicationProcess("UART Communication Proccess", process_stack, process_priority);
    this->_uart_communication_process->InstallDriver(
        new UARTDriver(UART_NUM_0, Baudrate::BaudRate115200, 1024));
    this->_uart_communication_process->Configure(0x0000, ProtobufIndex::UART_TRANSMIT);

    result += this->_shared_memory_manager->SignUpSharedArea(ProtobufIndex::UART_TRANSMIT, CommunicationProtobuf::GetStaticMaxSize(), READ_WRITE);

    this->_uart_communication_process->InitializeProcess();

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
esp_err_t Kernel::EnableLoraProcess(uint32_t process_stack, uint8_t process_priority, bool can_fail) {
    auto result = ESP_OK;

    do {
        if ((!this->_spi_initialized) || (!this->_gpio_initialized)) {
            result = ESP_FAIL;
            break;
        }

        this->_lora_communication_process = new CommunicationProcess("LoRa Communication Proccess", process_stack, process_priority);
        this->_lora_communication_process->InstallDriver(
            new LoRaDriver(Regions::BRAZIL, CRCMode::DISABLE, 255));
        this->_lora_communication_process->Configure(0x0000, ProtobufIndex::UART_TRANSMIT);

        result += this->_shared_memory_manager->SignUpSharedArea(ProtobufIndex::LORA_TRANSMIT, CommunicationProtobuf::GetStaticMaxSize(), READ_WRITE);

        this->_lora_communication_process->InitializeProcess();

    } while (0);

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
esp_err_t Kernel::EnableMQTTClientProcess(uint32_t process_stack, uint8_t process_priority, bool can_fail) {
    auto result = ESP_OK;

    do {
        this->_mqtt_client_process = new MQTTClientProcess("MQTT Client Proccess", process_stack, process_priority);
        result += this->_shared_memory_manager->SignUpSharedArea(ProtobufIndex::MQTT, MQTTClientProtobuf::GetStaticMaxSize(), READ_WRITE);
        this->_mqtt_client_process->InitializeProcess();

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
void Kernel::InjectDebugCredentials(const char* ssid, const char* password) {
    CredentialsProtobuf credentials_debug{};

    credentials_debug.UpdateSsid(const_cast<char*>(ssid));          // TODO: implement a safe strlen, strcpy and strcmp
    credentials_debug.UpdatePassword(const_cast<char*>(password));  // TODO: implement a safe strlen, strcpy and strcmp

    this->_shared_memory_manager->Write(ProtobufIndex::CREDENTIALS, &credentials_debug);
}
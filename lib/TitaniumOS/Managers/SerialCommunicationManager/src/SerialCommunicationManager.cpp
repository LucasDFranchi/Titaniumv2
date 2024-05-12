#include "./../inc/SerialCommunicationManager.h"

#include "HAL/memory/SharedMemoryManager.h"
#include "Protocols/Titanium/TitaniumProtocol.h"

/**
 * @brief Stores a TitaniumPackage into the shared memory.
 *
 * @param[in] package The package to store.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t SerialCommunicationManager::StorePackage(std::unique_ptr<TitaniumPackage>& package) {
    auto payload = std::make_unique<uint8_t[]>(package.get()->size());

    package.get()->Consume(payload.get());

    return this->_shared_memory_manager.get()->Write(package.get()->memory_area(), package.get()->size(), payload.get());
}

/**
 * @brief Generates a response package from the shared memory.
 *
 * @param[in] memory_area The memory area to read the response from.
 * @return A unique pointer to the generated TitaniumPackage.
 */
std::unique_ptr<TitaniumPackage> SerialCommunicationManager::GenerateResponsePackage(uint8_t memory_area) {
    auto response_size    = this->_shared_memory_manager.get()->GetAreaSize(memory_area);
    auto response_payload = std::make_unique<uint8_t[]>(response_size);

    this->_shared_memory_manager.get()->Read<uint8_t>(memory_area, response_payload.get());

    return std::make_unique<TitaniumPackage>(response_size, RESPONSE_COMMAND, memory_area, response_payload.get());
}

std::unique_ptr<TitaniumPackage> SerialCommunicationManager::GenerateTransmissionPackage(void) {
    auto response_size    = this->_shared_memory_manager.get()->GetAreaSize(this->_memory_area_transmit);
    auto response_payload = std::make_unique<uint8_t[]>(response_size);
    this->_shared_memory_manager.get()->Read<uint8_t>(this->_memory_area_transmit, response_payload.get());

    return std::make_unique<TitaniumPackage>(response_size, TRANSMISSION_COMMAND, this->_memory_area_receive, response_payload.get());
}

/**
 * @brief Sends an acknowledgment based on the result of an operation.
 *
 * @param[in] result The result of the operation.
 */
void SerialCommunicationManager::Acknowledge(esp_err_t result) {
    if (result == ESP_OK) {
        this->_driver.get()->Write(const_cast<uint8_t*>(ProtocolConstants::ACK), sizeof(ProtocolConstants::ACK));
    } else {
        this->_driver.get()->Write(const_cast<uint8_t*>(ProtocolConstants::NAK), sizeof(ProtocolConstants::NAK));
    }
}

/**
 * @brief Main execution loop for the SerialCommunicationManager.
 */
void SerialCommunicationManager::Execute(void) {
    auto protocol = TitaniumProtocol();

    if (this->Initialize() != ESP_OK) {
        vTaskDelete(this->_process_handler);
    }

    while (1) {
        if (this->_driver.get() == nullptr) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        auto received_bytes = this->_driver.get()->Read(this->_buffer.get());
        if (received_bytes > 0) {
            std::unique_ptr<TitaniumPackage> package = nullptr;

            auto result = protocol.Decode(this->_buffer.get(), received_bytes, package);
            if (result == ESP_OK) {
                switch (package.get()->command()) {
                    case READ_COMMAND: {
                        auto response_package     = this->GenerateResponsePackage(package.get()->memory_area());
                        auto response_buffer_size = protocol.Encode(response_package, this->_buffer.get(), this->_driver.get()->buffer_size());

                        result = this->_driver.get()->Write(this->_buffer.get(), response_buffer_size);
                        break;
                    }
                    case WRITE_COMMAND:
                    case TRANSMISSION_COMMAND: {
                        result = StorePackage(package);
                        break;
                    }
                    default: {
                        result = ESP_FAIL;
                    }
                }
            }

            this->Acknowledge(result);

        } 
        if (this->_shared_memory_manager.get()->IsAreaDataUpdated(this->_memory_area_transmit)) {
            auto response_package     = this->GenerateTransmissionPackage();
            auto response_buffer_size = protocol.Encode(response_package, this->_buffer.get(), this->_driver.get()->buffer_size());

            this->_driver.get()->Write(this->_buffer.get(), response_buffer_size);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Installs the driver interface and sets the memory area.
 *
 * @param[in] driver_interface The driver interface to use.
 * @param[in] memory_area The memory area to operate on.
 */
void SerialCommunicationManager::InstallDriver(IDriverInterface* driver_interface, uint8_t memory_area_receive, uint8_t memory_area_transmit) {
    this->_driver.reset(driver_interface);
    this->_buffer               = std::make_unique<uint8_t[]>(driver_interface->buffer_size());
    this->_memory_area_receive  = memory_area_receive;
    this->_memory_area_transmit = memory_area_transmit;
}

/**
 * @brief Initializes the SerialCommunicationManager.
 *
 * @return The result of the initialization process.
 */
esp_err_t SerialCommunicationManager::Initialize(void) {
    this->_shared_memory_manager.reset(SharedMemoryManager::GetInstance());
    return ESP_OK;
}
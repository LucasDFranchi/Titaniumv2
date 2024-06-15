#include "SerialCommunicationManager.h"

#include "Protocols/Titanium/TitaniumProtocol.h"
#include "HAL/memory/SharedMemoryManager.h"

esp_err_t SerialCommunicationManager::StorePackage(std::unique_ptr<TitaniumPackage>& package) {
    auto shared_memory_manager = SharedMemoryManager::GetInstance();
    auto payload               = std::make_unique<uint8_t[]>(package.get()->size());

    package.get()->Consume(payload.get());

    return shared_memory_manager->Write(package.get()->memory_area(), package.get()->size(), payload.get());
}

std::unique_ptr<TitaniumPackage> SerialCommunicationManager::GenerateResponsePackage(uint8_t memory_area) {
    auto shared_memory_manager = SharedMemoryManager::GetInstance();
    auto response_size         = shared_memory_manager->GetAreaSize(memory_area);
    auto response_payload      = std::make_unique<uint8_t[]>(response_size);

    shared_memory_manager->Read<uint8_t>(memory_area, response_payload.get());

    return std::make_unique<TitaniumPackage>(response_size, RESPONSE_COMMAND, memory_area, response_payload.get());
}

void SerialCommunicationManager::Acknowledge(esp_err_t result) {
    if (result == ESP_OK) {
        this->_driver.get()->Write(const_cast<uint8_t*>(ProtocolConstants::ACK), sizeof(ProtocolConstants::ACK));
    } else {
        this->_driver.get()->Write(const_cast<uint8_t*>(ProtocolConstants::NAK), sizeof(ProtocolConstants::NAK));
    }
}

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

        auto received_bytes = this->_driver.get()->Read(this->buffer.get());
        if (received_bytes > 0) {
            std::unique_ptr<TitaniumPackage> package = nullptr;

            auto result = protocol.Decode(this->buffer.get(), received_bytes, package);

            if (result == ESP_OK) {
                if (package.get()->command() == READ_COMMAND) {
                    auto response_package     = this->GenerateResponsePackage(package.get()->memory_area());
                    auto response_buffer_size = protocol.Encode(response_package, this->buffer.get(), this->_driver.get()->buffer_size());

                    result = this->_driver.get()->Write(this->buffer.get(), response_buffer_size);

                } else if (package.get()->command() == WRITE_COMMAND) {
                    result = StorePackage(package);
                }
            }

            this->Acknowledge(result);

            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

void SerialCommunicationManager::InstallDriver(IDriverInterface* driver_interface) {
    this->_driver.reset(driver_interface);
    this->buffer = std::make_unique<uint8_t[]>(driver_interface->buffer_size());
}

/**
 * Initializes the SerialCommunicationManager.
 *
 * @returns The result of the initialization process.
 */
esp_err_t SerialCommunicationManager::Initialize(void) {
    return ESP_OK;  // Maybe remove this
}
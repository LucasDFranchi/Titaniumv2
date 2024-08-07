#include "./../inc/CommunicationProcess.h"

#include "HAL/memory/SharedMemoryManager.h"
#include "Protocols/Titanium/TitaniumProtocol.h"

/**
 * @brief Stores a TitaniumPackage into the shared memory.
 *
 * @param[in] package The package to store.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t CommunicationProcess::StorePackage(std::unique_ptr<TitaniumPackage>& package) {
    uint8_t payload[package.get()->size()] = {0};
    package.get()->Consume(payload);

    return this->_shared_memory_manager.get()->Write(package.get()->memory_area(), package.get()->size(), payload);
}

/**
 * @brief Generates a response package from the shared memory.
 *
 * @param[in] memory_area The memory area to read the response from.
 * @return A unique pointer to the generated TitaniumPackage.
 */
std::unique_ptr<TitaniumPackage> CommunicationProcess::GenerateResponsePackage(uint8_t memory_area) {
    auto response_size    = this->_shared_memory_manager.get()->GetWrittenBytes(memory_area);
    auto response_payload = std::make_unique<uint8_t[]>(response_size);

    this->_shared_memory_manager.get()->Read(memory_area, response_size, response_payload.get());

    return std::make_unique<TitaniumPackage>(response_size, this->_address, RESPONSE_COMMAND, memory_area, response_payload.get());
}

/**
 * @brief Generates a transmission package.
 *
 * @return A unique pointer to the generated TitaniumPackage.
 */
std::unique_ptr<TitaniumPackage> CommunicationProcess::GenerateTransmissionPackage(CommunicationProtobuf& communication_proto) {
    std::unique_ptr<uint8_t[]> titanium_package = nullptr;
    auto response_size                          = this->_shared_memory_manager.get()->GetWrittenBytes(communication_proto.GetReadMemoryArea());
    auto response_payload                       = std::make_unique<uint8_t[]>(response_size);

    memset_s<uint8_t>(response_payload.get(), 0, response_size);
    this->_shared_memory_manager.get()->Read(communication_proto.GetReadMemoryArea(), response_size, response_payload.get());

    return std::make_unique<TitaniumPackage>(response_size,
                                             this->_address,
                                             static_cast<command_e>(communication_proto.GetCommand()),
                                             communication_proto.GetWriteMemoryArea(),
                                             response_payload.get());
}

/**
 * @brief Sends an acknowledgment based on the result of an operation.
 *
 * @param[in] result The result of the operation.
 */
void CommunicationProcess::Acknowledge(esp_err_t result) {
    if (result == ESP_OK) {
        this->_driver.get()->Write(const_cast<uint8_t*>(ProtocolConstants::ACK), sizeof(ProtocolConstants::ACK));
    } else {
        this->_driver.get()->Write(const_cast<uint8_t*>(ProtocolConstants::NAK), sizeof(ProtocolConstants::NAK));
    }
}

/**
 * @brief Main execution loop for the CommunicationProcess.
 */
void CommunicationProcess::Execute(void) {
    auto protocol            = TitaniumProtocol();
    auto communication_proto = CommunicationProtobuf();

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
                if (this->_address != package.get()->address()) {
                    continue;
                }

                switch (package.get()->command()) {
                    case READ_COMMAND: {
                        auto response_package     = this->GenerateResponsePackage(package.get()->memory_area());
                        auto response_buffer_size = protocol.Encode(response_package, this->_buffer.get(), this->_driver.get()->buffer_size());

                        result = this->_driver.get()->Write(this->_buffer.get(), response_buffer_size);
                        break;
                    }
                    case RESPONSE_COMMAND: {
                        // TODO: Do something here!
                        /*
                         * Here there is two options, the first one is to override the requested area, or store this data into a temp share memory buffer
                         */
                        break;
                    }
                    case ACK_COMMAND: {
                        // TODO: Do something here!
                        /*
                         * Here there is two options, the first one is to override the requested area, or store this data into a temp share memory buffer
                         */
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
            this->_shared_memory_manager.get()->Read(this->_memory_area_transmit, &communication_proto);

            auto response_package = this->GenerateTransmissionPackage(communication_proto);

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
 * @param[in] memory_area_receive The memory area to receive data.
 * @param[in] memory_area_transmit The memory area to transmit data.
 */
void CommunicationProcess::InstallDriver(IDriverInterface* driver_interface) {
    this->_driver.reset(driver_interface);
    this->_buffer = std::make_unique<uint8_t[]>(driver_interface->buffer_size());
    memset_s<uint8_t>(this->_buffer.get(), 0, driver_interface->buffer_size());
}

void CommunicationProcess::Configure(uint16_t address, uint8_t memory_area_transmit) {
    this->_address              = address;
    this->_memory_area_transmit = memory_area_transmit;
}

/**
 * @brief Initializes the CommunicationProcess.
 *
 * @return The result of the initialization process.
 */
esp_err_t CommunicationProcess::Initialize(void) {
    this->_shared_memory_manager.reset(SharedMemoryManager::GetInstance());
    return ESP_OK;
}

#include "./../inc/CommunicationProcess.h"

#include "HAL/memory/SharedMemoryManager.h"
#include "Protocols/Titanium/TitaniumProtocol.h"

#include "esp_log.h"

// /**
//  * @brief Stores a TitaniumPackage into the shared memory.
//  *
//  * @param[in] package The package to store.
//  * @return ESP_OK on success, or an error code on failure.
//  */
// esp_err_t CommunicationProcess::StorePackage(std::unique_ptr<TitaniumPackage>& package) {
//     uint8_t payload[package.get()->size()] = {0};
//     package.get()->Consume(payload);

//     return this->_shared_memory_manager.get()->Write(package.get()->memory_area(), package.get()->size(), payload);
// }

// /**
//  * @brief Generates a response package from the shared memory.
//  *
//  * @param[in] memory_area The memory area to read the response from.
//  * @return A unique pointer to the generated TitaniumPackage.
//  */
// std::unique_ptr<TitaniumPackage> CommunicationProcess::GenerateResponsePackage(uint8_t memory_area) {
//     auto response_size    = this->_shared_memory_manager.get()->GetWrittenBytes(memory_area);
//     auto response_payload = std::make_unique<uint8_t[]>(response_size);

//     this->_shared_memory_manager.get()->Read(memory_area, response_size, response_payload.get());

//     return std::make_unique<TitaniumPackage>(response_size, this->_address, RESPONSE_COMMAND, memory_area, response_payload.get());
// }

/**
 * @brief Generates a transmission package.
 *
 * @return A unique pointer to the generated TitaniumPackage.
 */
std::unique_ptr<TitaniumPackage> CommunicationProcess::GenerateTransmitPackage(CommunicationProtobuf& communication_proto) {
    return std::make_unique<TitaniumPackage>(strlen(communication_proto.GetPayload()) + 1,
                                             this->_address,
                                             static_cast<command_e>(communication_proto.GetCommand()),
                                             communication_proto.GetMemoryArea(),
                                             reinterpret_cast<uint8_t*>(const_cast<char*>((communication_proto.GetPayload()))));
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
    auto protocol       = TitaniumProtocol();
    auto transmit_proto = CommunicationProtobuf();
    auto receive_proto  = CommunicationProtobuf();

    if (this->Initialize() != ESP_OK) {
        vTaskDelete(this->_process_handler);
    }

    while (1) {
        if (this->_driver.get() == nullptr) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        auto received_bytes = this->_driver.get()->Read(this->_buffer.get());

        if (received_bytes == 0) {
            if (this->_shared_memory_manager->IsAreaDataUpdated(ProtobufIndex::LORA_TRANSMIT)) {
                this->_shared_memory_manager->Read(ProtobufIndex::LORA_TRANSMIT, &transmit_proto);

                auto package              = this->GenerateTransmitPackage(transmit_proto);
                auto response_buffer_size = protocol.Encode(package, this->_buffer.get(), this->_driver.get()->buffer_size());
                this->_driver.get()->Write(this->_buffer.get(), response_buffer_size);
            }
        } else {
            std::unique_ptr<TitaniumPackage> package = nullptr;
            auto result                              = protocol.Decode(this->_buffer.get(), received_bytes, package);

            if (result != ESP_OK) {
                ESP_LOGI("Communication Process", "Decode Error: %d", result);
                vTaskDelay(pdMS_TO_TICKS(100));
                continue;
            }

            if (this->CheckAddressPackage(package.get()->address())) {
                ESP_LOGI("Communication Process", "Error Address");
                vTaskDelay(pdMS_TO_TICKS(100));
                continue;
            }
            ESP_LOGI("Communication Process", "Right Address");

            switch (package.get()->command()) {
                case READ_COMMAND: {
                    ESP_LOGI("Communication Protocol", "READ_COMMAND");
                    char response_buffer[200] = {0};  // Add something in the proto to get the string maximum size;
                    auto protobuf             = ProtobufFactory::CreateProtobuf(package.get()->memory_area());

                    if (protobuf.get() == nullptr) {
                        ESP_LOGI("Communication Protocol", "NULL PROTOBUF");
                        break;
                    }
                    this->_shared_memory_manager->Read(package.get()->memory_area(), protobuf.get());
                    uint16_t response_buffer_size = protobuf->SerializeJson(response_buffer, sizeof(response_buffer));
                    if (response_buffer_size <= 0) {
                        ESP_LOGI("Communication Protocol", "INVALID JSON");
                        break;
                    }
                    transmit_proto.UpdateAddress(this->_address);
                    transmit_proto.UpdateCommand(READ_RESPONSE_COMMAND);
                    transmit_proto.UpdateMemoryArea(ProtobufIndex::LORA_RECEIVE);
                    transmit_proto.UpdatePayload(response_buffer, response_buffer_size);

                    auto response_package = this->GenerateTransmitPackage(transmit_proto);
                    response_buffer_size = protocol.Encode(response_package, this->_buffer.get(), this->_driver.get()->buffer_size());
                    ESP_LOGI("Communication Protocol", "WRITE RESPONSE");
                    this->_driver.get()->Write(this->_buffer.get(), response_buffer_size);
                    break;
                }
                case READ_RESPONSE_COMMAND: {
                    ESP_LOGI("Communication Protocol", "READ_RESPONSE_COMMAND");
                    // ADD p´rints here
                    break;
                }
                case ACK_COMMAND: {
                    ESP_LOGI("Communication Protocol", "ACK_COMMAND");
                    break;
                }
                case NACK_COMMAND: {
                    ESP_LOGI("Communication Protocol", "NACK_COMMAND");
                    break;
                }
                case WRITE_COMMAND: {
                    ESP_LOGI("Communication Protocol", "WRITE_COMMAND");
                    char response_buffer[200] = {0};  // Add something in the proto to get the string maximum size;
                    auto protobuf             = ProtobufFactory::CreateProtobuf(package.get()->memory_area());

                    if (protobuf.get() == nullptr) {
                        break;
                    }

                    package.get()->Consume(reinterpret_cast<uint8_t*>(response_buffer));

                    // uint16_t response_buffer_size = protobuf->DeSerializeJson(response_buffer, sizeof(response_buffer));
                    // this->_shared_memory_manager->Write(package.get()->memory_area(), protobuf.get());
                    break;
                }
                default: {
                    result = ESP_FAIL;
                }
            }

            //     // this->ProcessReceivedPackage(package);
        }

        // if (result == ESP_OK) {
        //

        // }

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
 * @brief Check if the received package is destined to this device.
 *
 * @param[in] address Address of the received package.
 *
 * @return The ESP_OK if the address matches or if it's a broadcast package.
 */
bool CommunicationProcess::CheckAddressPackage(uint16_t address) {
    auto result = ESP_FAIL;

    if ((address == this->_address) ||
        (address == ProtocolConstants::BROADCAST_ADDRESS)) {
        result = ESP_OK;
    }

    return result;
}

/**
 * @brief Initializes the CommunicationProcess.
 *
 * @return The result of the initialization process.
 */
esp_err_t CommunicationProcess::Initialize(void) {
    this->_shared_memory_manager.reset(SharedMemoryManager::GetInstance());
    if (SharedMemoryManager::GetInstance() == nullptr) {
        ESP_LOGI("Communication Protocol", "Init NULL");
    }
    return ESP_OK;
}

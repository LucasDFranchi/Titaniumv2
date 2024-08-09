#include "./../inc/CommunicationProcess.h"

#include "HAL/memory/SharedMemoryManager.h"

#include "esp_log.h"

namespace CommunicationConstants {
    constexpr uint8_t MAXIMUM_ACK_RETRIES = 3;
}

/**
 * @brief Generates a transmission package.
 *
 * @return A unique pointer to the generated TitaniumPackage.
 */
std::unique_ptr<TitaniumPackage> CommunicationProcess::GenerateTransmitPackage(CommunicationProtobuf& communication_proto) {
    return std::make_unique<TitaniumPackage>(this->_address,
                                             static_cast<command_e>(communication_proto.GetCommand()),
                                             communication_proto.GetMemoryArea(),
                                             communication_proto);
}

// /**
//  * @brief Sends an acknowledgment based on the result of an operation.
//  *
//  * @param[in] result The result of the operation.
//  */
// void CommunicationProcess::Acknowledge(esp_err_t result) {
//     if (result == ESP_OK) {
//         this->_driver.get()->Write(const_cast<uint8_t*>(ProtocolConstants::ACK), sizeof(ProtocolConstants::ACK));
//     } else {
//         this->_driver.get()->Write(const_cast<uint8_t*>(ProtocolConstants::NAK), sizeof(ProtocolConstants::NAK));
//     }
// }

/**
 * @brief Main execution loop for the CommunicationProcess.
 */
void CommunicationProcess::Execute(void) {
    if (this->Initialize() != ESP_OK) {
        vTaskDelete(this->_process_handler);
    }

    while (1) {
        if (this->_driver.get() == nullptr) {
            vTaskDelay(pdMS_TO_TICKS(1000));  // Not sure if this make sense, hot reload the driver?
            continue;
        }
        auto received_bytes = this->_driver.get()->Read(this->_buffer.get());

        if (received_bytes == 0) {
            if (this->_shared_memory_manager->IsAreaDataUpdated(ProtobufIndex::LORATRANSMIT)) {
                this->_shared_memory_manager->Read(ProtobufIndex::LORATRANSMIT, *this->_transmit_proto);

                auto package              = this->GenerateTransmitPackage(*this->_transmit_proto);
                auto response_buffer_size = this->_protocol->Encode(package, this->_buffer.get(), this->_driver.get()->buffer_size());
                this->_driver.get()->Write(this->_buffer.get(), response_buffer_size);
            }
        } else {
            // std::unique_ptr<TitaniumPackage> package = nullptr;
            // auto result                              = this->_protocol->Decode(this->_buffer.get(), received_bytes, package);

            // do {
            //     if (result != ESP_OK) {
            //         break;
            //     }

            //     if (this->CheckAddressPackage(package.get()->address())) {
            //         break;
            //     }

            // } while (0);

            vTaskDelay(pdMS_TO_TICKS(100));

            // switch (package.get()->command()) {
            //     case READ_COMMAND: {
            //         char response_buffer[200] = {0};  // Add something in the proto to get the string maximum size;
            //         auto protobuf             = ProtobufFactory::CreateProtobuf(package.get()->memory_area());

            //         if (protobuf.get() == nullptr) {
            //             break;
            //         }
            //         this->_shared_memory_manager->Read(package.get()->memory_area(), protobuf.get());
            //         uint16_t response_buffer_size = protobuf->SerializeJson(response_buffer, sizeof(response_buffer));
            //         if (response_buffer_size <= 0) {
            //             break;
            //         }
            //         transmit_proto.UpdateAddress(this->_address);
            //         transmit_proto.UpdateCommand(READ_RESPONSE_COMMAND);
            //         transmit_proto.UpdateMemoryArea(ProtobufIndex::LORA_RECEIVE);
            //         transmit_proto.UpdatePayload(response_buffer, response_buffer_size);

            //         auto response_package = this->GenerateTransmitPackage(transmit_proto);
            //         response_buffer_size = protocol.Encode(response_package, this->_buffer.get(), this->_driver.get()->buffer_size());
            //         this->_driver.get()->Write(this->_buffer.get(), response_buffer_size);
            //         break;
            //     }
            //     case READ_RESPONSE_COMMAND: {
            //         ESP_LOGI("Communication Protocol", "READ_RESPONSE_COMMAND");

            //         break;
            //     }
            //     case ACK_COMMAND: {
            //         ESP_LOGI("Communication Protocol", "ACK_COMMAND");
            //         break;
            //     }
            //     case NACK_COMMAND: {
            //         ESP_LOGI("Communication Protocol", "NACK_COMMAND");
            //         break;
            //     }
            //     case WRITE_COMMAND: {
            //         ESP_LOGI("Communication Protocol", "WRITE_COMMAND");
            //         char response_buffer[200] = {0};  // Add something in the proto to get the string maximum size;
            //         auto protobuf             = ProtobufFactory::CreateProtobuf(package.get()->memory_area());

            //         if (protobuf.get() == nullptr) {
            //             break;
            //         }

            //         package.get()->Consume(reinterpret_cast<uint8_t*>(response_buffer));

            //         // uint16_t response_buffer_size = protobuf->DeSerializeJson(response_buffer, sizeof(response_buffer));
            //         // this->_shared_memory_manager->Write(package.get()->memory_area(), protobuf.get());
            //         break;
            //     }
            //     default: {
            //         result = ESP_FAIL;
            //     }
            // }

            //     // this->ProcessReceivedPackage(package);
        }

        // if (result == ESP_OK) {
        //

        // }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

esp_err_t CommunicationProcess::InstallDriver(IDriverInterface* driver_interface) {
    if (driver_interface == nullptr) {
        return ESP_FAIL;
    }

    this->_driver.reset(driver_interface);
    this->_buffer = std::make_unique<uint8_t[]>(driver_interface->buffer_size());

    memset_s<uint8_t>(this->_buffer.get(), 0, driver_interface->buffer_size());

    return ESP_OK;
}

void CommunicationProcess::Configure(uint16_t address) {  // Update to set address
    this->_address = address;
}

esp_err_t CommunicationProcess::ProcessReceivedPackage(std::unique_ptr<TitaniumPackage>& package) {
    auto result = ESP_FAIL;

    switch (package.get()->command()) {
        case READ_COMMAND: {
            ESP_LOGI("Communication Process", "READ_COMMAND");
            result = this->ProcessReadPackage(package);
            break;
        }
        case READ_RESPONSE_COMMAND: {
            ESP_LOGI("Communication Process", "READ_RESPONSE_COMMAND");
            result = this->ProcessReadResponsePackage(package, false);

            break;
        }
        case ACK_COMMAND: {
            ESP_LOGI("Communication Process", "ACK_COMMAND");
            result = this->ProcessAckPackage(package);
            break;
        }
        case NACK_COMMAND: {
            ESP_LOGI("Communication Process", "NACK_COMMAND");
            result = this->ProcessNackPackage(package);
            break;
        }
        case WRITE_COMMAND: {
            ESP_LOGI("Communication Process", "WRITE_COMMAND");
            result = this->ProcessWritePackage(package, false);
            break;
        }
        case READ_RESPONSE_SECURE_COMMAND: {
            ESP_LOGI("Communication Process", "READ_RESPONSE_SECURE_COMMAND");
            result = this->ProcessWritePackage(package, true);
            break;
        }
        case WRITE_SECURE_COMMAND: {
            ESP_LOGI("Communication Process", "WRITE_SECURE_COMMAND");
            result = this->ProcessWritePackage(package, true);
            break;
        }
        default: {
            result = ESP_FAIL;
        }
    }

    return result;
}

esp_err_t CommunicationProcess::ProcessReadPackage(std::unique_ptr<TitaniumPackage>& package) {
    char response_buffer[200] = {0};  // Add something in the proto to get the string maximum size;
    auto protobuf             = ProtobufFactory::CreateProtobuf(package.get()->memory_area());
    auto result               = ESP_FAIL;

    do {
        if (protobuf.get() == nullptr) {
            // Add a error for missing protobuf
            break;
        }
        this->_shared_memory_manager->Read(package.get()->memory_area(), *protobuf);

        uint16_t response_buffer_size = protobuf->Serialize(response_buffer, sizeof(response_buffer));
        if (response_buffer_size <= 0) {
            break;
        }

        this->_transmit_proto->UpdateAddress(this->_address);
        this->_transmit_proto->UpdateCommand(READ_RESPONSE_COMMAND);
        this->_transmit_proto->UpdateMemoryArea(ProtobufIndex::LORARECEIVE);
        this->_transmit_proto->UpdatePayload(response_buffer, response_buffer_size);

        auto response_package = this->GenerateTransmitPackage(*this->_transmit_proto);
        if (response_package.get() == nullptr) {
            // Add a error for wrong package generation
            break;
        }

        response_buffer_size = this->_protocol->Encode(response_package, this->_buffer.get(), this->_driver.get()->buffer_size());
        if (response_package.get() == nullptr) {
            // Add a error for wrong package generation
            break;
        }

        result = this->_driver.get()->Write(this->_buffer.get(), response_buffer_size);
    } while (0);

    return result;
}

esp_err_t CommunicationProcess::ProcessWritePackage(std::unique_ptr<TitaniumPackage>& package, bool should_ack) {
    char response_buffer[200] = {0};  // Add something in the proto to get the string maximum size;
    auto protobuf             = ProtobufFactory::CreateProtobuf(package.get()->memory_area());
    auto result               = ESP_FAIL;

    do {
        if (protobuf.get() == nullptr) {
            break;
        }

        auto raw_size = package.get()->Consume(reinterpret_cast<uint8_t*>(response_buffer));

        if (raw_size == 0) {
            ESP_LOGI("Communication Process", "Error");
            // Add a error for wrong package generation
            break;
        }
        auto response_buffer_size = protobuf->DeSerialize(response_buffer, sizeof(response_buffer));
        if (response_buffer_size != ESP_OK) {
            // Add a error for wrong package generation
            break;
        }

        result = this->_shared_memory_manager->Write(package.get()->memory_area(), *protobuf);

        if (result != ESP_OK) {
            // Add a error for package write
            break;
        }

        // if (should_ack) {
        //     this->package->uuid
        // }

    } while (0);

    return result;
}

esp_err_t CommunicationProcess::ProcessReadResponsePackage(std::unique_ptr<TitaniumPackage>& package, bool should_ack) {
    return ESP_OK;
}

esp_err_t CommunicationProcess::ProcessAckPackage(std::unique_ptr<TitaniumPackage>& package) {
    return ESP_OK;
}

esp_err_t CommunicationProcess::ProcessNackPackage(std::unique_ptr<TitaniumPackage>& package) {
    return ESP_OK;
}

/**
 * @brief Initializes the CommunicationProcess.
 *
 * @return The result of the initialization process.
 */
esp_err_t CommunicationProcess::Initialize(void) {
    this->_shared_memory_manager.reset(SharedMemoryManager::GetInstance());

    if (SharedMemoryManager::GetInstance() == nullptr) {
        ESP_LOGE("Communication Protocol", "Shared Memory Manager not Initialized");

        return ESP_FAIL;
    }

    this->_protocol.reset(new TitaniumProtocol());
    this->_transmit_proto.reset(new CommunicationProtobuf());
    this->_receive_proto.reset(new CommunicationProtobuf());

    return ESP_OK;
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

bool CommunicationProcess::HasTransmissionPending(void) {
    return this->_shared_memory_manager->IsAreaDataUpdated(ProtobufIndex::LORATRANSMIT);
}

bool CommunicationProcess::HasReceivedBytes(void) {
    this->_received_bytes = this->_driver.get()->Read(this->_buffer.get());
    return this->_received_bytes > 0;
}

void CommunicationProcess::ProcessState(void) {
}

CommunicationProcess::State CommunicationProcess::Idle(void) {
    auto next_state = this->communication_state;

    if (this->HasReceivedBytes()) {
        next_state = State::PASSIVE;
    } else if (this->HasTransmissionPending()) {
        next_state = State::ACTIVE;
    }

    return next_state;
}

CommunicationProcess::State CommunicationProcess::Passive(void) {
    std::unique_ptr<TitaniumPackage> package = nullptr;
    auto next_state                          = this->communication_state;
    auto result                              = this->_protocol->Decode(this->_buffer.get(), this->_received_bytes, package);

    this->_received_bytes = 0;

    if (result != ESP_OK) {
        next_state = State::IDLE;
    }

    if (!this->CheckAddressPackage(package.get()->address())) {
        this->ProcessReceivedPackage(package);
    } else {
        /* Case in the future we support Daisy chain we need to
         * update this to a forwarding state.
         */
    }

    next_state = State::IDLE;
    return next_state;
}

CommunicationProcess::State CommunicationProcess::Active(void) {
    auto next_state = this->communication_state;

    this->_shared_memory_manager->Read(ProtobufIndex::LORATRANSMIT, *this->_transmit_proto);
    auto package              = this->GenerateTransmitPackage(*this->_transmit_proto);
    auto response_buffer_size = this->_protocol->Encode(package, this->_buffer.get(), this->_driver.get()->buffer_size());
    this->_driver.get()->Write(this->_buffer.get(), response_buffer_size);

    next_state = State::IDLE;
    return next_state;
}
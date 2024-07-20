#include "SystemProcess/CommunicationProcess/inc/CommunicationProcess.h"
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

/**
 * @brief Main execution loop for the CommunicationProcess.
 */
void CommunicationProcess::Execute(void) {
    if (this->Initialize() != ESP_OK) {
        vTaskDelete(this->_process_handler);
    }

    while (1) {
        this->ProcessState();

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void CommunicationProcess::ProcessState(void) {
    auto next_state = this->communication_state;

    switch (this->communication_state) {
        case State::IDLE:
            next_state = this->Idle();
            break;
        case State::REACTIVE:
            next_state = this->Reactive();
            break;
        case State::ACTIVE:
            next_state = this->Active();
            break;

        default:
            next_state = State::IDLE;
            break;
    }

    if (next_state != this->communication_state) {
        ESP_LOGI("Communication Process", "State transition from %d to %d",
                 static_cast<int>(this->communication_state),
                 static_cast<int>(next_state));
        this->communication_state = next_state;
    }
}

CommunicationProcess::State CommunicationProcess::Idle(void) {
    auto next_state = this->communication_state;

    if (this->HasReceivedBytes()) {
        next_state = State::REACTIVE;
    } else if (this->HasTransmissionPending()) {
        next_state = State::ACTIVE;
    }

    return next_state;
}

CommunicationProcess::State CommunicationProcess::Reactive(void) {
    std::unique_ptr<TitaniumPackage> package = nullptr;
    auto result                              = this->_protocol->Decode(this->_buffer.get(), this->_received_bytes, package);

    this->_received_bytes = 0;

    if (result == ESP_OK) {
        if (!this->CheckAddressPackage(package.get()->address())) {
            this->ProcessReceivedPackage(package);
        } else {
            /* Case in the future we support Daisy chain we need to
             * update this to a forwarding state.
             */
            ESP_LOGI("Communication Process", "Forwarding");
        }
    }

    return State::IDLE;
}

CommunicationProcess::State CommunicationProcess::Active(void) {
    this->_shared_memory_manager->Read(ProtobufIndex::LORA_TRANSMIT, *this->_transmit_proto);

    auto package              = this->GenerateTransmitPackage(*this->_transmit_proto);
    auto response_buffer_size = this->_protocol->Encode(package, this->_buffer.get(), this->_driver.get()->buffer_size());

    this->_driver.get()->Write(this->_buffer.get(), response_buffer_size);

    if ((this->_transmit_proto->GetCommand() == READ_SECURE_COMMAND) ||
        (this->_transmit_proto->GetCommand() == WRITE_SECURE_COMMAND)) {
        for (uint16_t i = 0; i < this->_ack_size_list; i++) {
            if (this->ack_list[i] == 0) {
                this->ack_list[i] = package->uuid();
                break;
            }
        }
    }

    return State::IDLE;
}

titan_err_t CommunicationProcess::InstallDriver(IDriverInterface* driver_interface,
                                                uint16_t transmit_area,
                                                uint16_t received_area) {
    if (driver_interface == nullptr) {
        return Error::UNKNOW_FAIL;
    }

    this->_driver.reset(driver_interface);
    this->_buffer = std::make_unique<uint8_t[]>(driver_interface->buffer_size());

    memset_s(this->_buffer.get(), 0, driver_interface->buffer_size());
    this->_transmit_area = transmit_area;
    this->_receive_area = received_area;

    return ESP_OK;
}

void CommunicationProcess::Configure(uint16_t address) {  // Update to set address
    this->_address = address;
}

titan_err_t CommunicationProcess::ProcessReceivedPackage(std::unique_ptr<TitaniumPackage>& package) {
    auto result = Error::UNKNOW_FAIL;

    switch (package.get()->command()) {
        case READ_COMMAND: {
            ESP_LOGI("Communication Process", "READ_COMMAND");  // Should not send ACK
            result = this->ProcessReadPackage(package, false);
            break;
        }
        case READ_SECURE_COMMAND: {
            ESP_LOGI("Communication Process", "READ_SECURE_COMMAND");  // Should not send ACK
            result = this->ProcessReadPackage(package, true);
            break;
        }
        case WRITE_COMMAND: {
            ESP_LOGI("Communication Process", "WRITE_COMMAND");  // Should not send ACK
            result = this->ProcessWritePackage(package, false);
            break;
        }
        case WRITE_SECURE_COMMAND: {
            ESP_LOGI("Communication Process", "WRITE_SECURE_COMMAND");  // Should send ACK
            result = this->ProcessWritePackage(package, true);
            break;
        }
        case READ_RESPONSE_COMMAND: {
            ESP_LOGI("Communication Process", "READ_RESPONSE_COMMAND");  // Should not send ACK
            result = this->ProcessReadResponsePackage(package, false);
            break;
        }
        case READ_RESPONSE_SECURE_COMMAND: {
            ESP_LOGI("Communication Process", "READ_RESPONSE_SECURE_COMMAND");  // Should send ACK
            result = this->ProcessReadResponsePackage(package, true);
            break;
        }
        case ACK_COMMAND: {
            ESP_LOGI("Communication Process", "ACK_COMMAND");
            result = this->ProcessAckPackage(package);
            break;
        }
        default: {
            ESP_LOGI("Communication Process", "INVALID COMMAND");
            result = Error::INVALID_COMMAND;
        }
    }

    return result;
}

titan_err_t CommunicationProcess::ProcessReadPackage(std::unique_ptr<TitaniumPackage>& package, bool should_ack) {
    char response_buffer[256] = {0};  // Add something in the proto to get the string maximum size;
    auto protobuf             = ProtobufFactory::CreateProtobuf(package.get()->memory_area());
    auto result               = Error::UNKNOW_FAIL;

    do {
        if (protobuf.get() == nullptr) {
            result = Error::NULL_PTR;
            break;
        }

        result = this->_shared_memory_manager->Read(package.get()->memory_area(), *protobuf);
        if (result != ESP_OK) {
            result = Error::READ_FAIL;
            break;
        }

        uint16_t response_buffer_size = protobuf->SerializeJson(response_buffer, sizeof(response_buffer));
        if (response_buffer_size <= 0) {
            result = Error::SERIALIZE_JSON_ERROR;
            break;
        }

        auto command_response = should_ack ? READ_RESPONSE_SECURE_COMMAND : READ_RESPONSE_COMMAND;
        auto response_package = std::make_unique<TitaniumPackage>(response_buffer_size,
                                                                  this->_address,
                                                                  command_response,
                                                                  this->_receive_area,
                                                                  reinterpret_cast<uint8_t*>(response_buffer));

        if (response_package.get() == nullptr) {
            result = Error::PACKAGE_GENERATION_ERROR;
            break;
        }

        response_buffer_size = this->_protocol->Encode(response_package,
                                                       this->_buffer.get(),
                                                       this->_driver.get()->buffer_size());
        if (response_buffer_size == 0) {
            result = Error::PACKAGE_ENCODE_ERROR;
            break;
        }

        result = this->_driver.get()->Write(this->_buffer.get(), response_buffer_size);
    } while (0);

    return result;
}

titan_err_t CommunicationProcess::ProcessWritePackage(std::unique_ptr<TitaniumPackage>& package, bool should_ack) {
    char response_buffer[200] = {0};  // Add something in the proto to get the string maximum size;
    auto protobuf             = ProtobufFactory::CreateProtobuf(package.get()->memory_area());
    auto result               = Error::UNKNOW_FAIL;

    do {
        if (protobuf.get() == nullptr) {
            result = Error::NULL_PTR;
            break;
        }

        auto raw_size = package.get()->Consume(reinterpret_cast<uint8_t*>(response_buffer));
        if (raw_size == 0) {
            result = Error::CONSUME_ERROR;
            break;
        }

        auto response_buffer_size = protobuf->DeSerialize(response_buffer, sizeof(response_buffer));
        if (response_buffer_size != ESP_OK) {
            result = Error::CONSUME_ERROR;
            break;
        }

        result = this->_shared_memory_manager->Write(package.get()->memory_area(), *protobuf);

        if (result != ESP_OK) {
            result = Error::DESERIALIZE_ERROR;
            break;
        }

        if (should_ack) {
            uint32_t uuid    = package->uuid();
            auto ack_package = std::make_unique<TitaniumPackage>(sizeof(uuid),
                                                                 this->_address,
                                                                 ACK_COMMAND,
                                                                 package.get()->memory_area(),
                                                                 reinterpret_cast<uint8_t*>(&uuid));
            if (ack_package.get() == nullptr) {
                result = Error::PACKAGE_GENERATION_ERROR;
                break;
            }

            auto ack_buffer_size = this->_protocol->Encode(ack_package,
                                                           this->_buffer.get(),
                                                           this->_driver.get()->buffer_size());
            if (ack_buffer_size == 0) {
                result = Error::PACKAGE_ENCODE_ERROR;
                break;
            }

            result = this->_driver.get()->Write(this->_buffer.get(), ack_buffer_size);
        }

    } while (0);

    return result;
}

titan_err_t CommunicationProcess::ProcessReadResponsePackage(std::unique_ptr<TitaniumPackage>& package, bool should_ack) {
    char response_buffer[256] = {0};  // Add something in the proto to get the string maximum size;
    auto result               = Error::UNKNOW_FAIL;

    do {
        auto raw_size = package.get()->Consume(reinterpret_cast<uint8_t*>(response_buffer));
        if (raw_size == 0) {
            result = Error::CONSUME_ERROR;
            break;
        }

        this->_receive_proto->UpdateAddress(package->address());
        this->_receive_proto->UpdateCommand(package->command());
        this->_receive_proto->UpdateMemoryArea(package->memory_area());
        this->_receive_proto->UpdatePayload(response_buffer, raw_size);

        if (should_ack) {
            uint32_t uuid    = package->uuid();
            auto ack_package = std::make_unique<TitaniumPackage>(sizeof(uuid),
                                                                 this->_address,
                                                                 ACK_COMMAND,
                                                                 package.get()->memory_area(),
                                                                 reinterpret_cast<uint8_t*>(&uuid));
            if (ack_package.get() == nullptr) {
                result = Error::PACKAGE_GENERATION_ERROR;
                break;
            }

            auto ack_buffer_size = this->_protocol->Encode(ack_package,
                                                           this->_buffer.get(),
                                                           this->_driver.get()->buffer_size());
            if (ack_buffer_size == 0) {
                result = Error::PACKAGE_ENCODE_ERROR;
                break;
            }

            result = this->_driver.get()->Write(this->_buffer.get(), ack_buffer_size);
        }

    } while (0);

    return result;
}

titan_err_t CommunicationProcess::ProcessAckPackage(std::unique_ptr<TitaniumPackage>& package) {
    auto uuid = 0;
    package->Consume(reinterpret_cast<uint8_t*>(&uuid));

    for (uint16_t i = 0; i < this->_ack_size_list; i++) {
        if (this->ack_list[i] == uuid) {
            this->ack_list[i] = 0;
            break;
        }
    }
    return ESP_OK;
}

/**
 * @brief Initializes the CommunicationProcess.
 *
 * @return The result of the initialization process.
 */
titan_err_t CommunicationProcess::Initialize(void) {
    this->_shared_memory_manager.reset(SharedMemoryManager::GetInstance());

    if (SharedMemoryManager::GetInstance() == nullptr) {
        ESP_LOGE("Communication Protocol", "Shared Memory Manager not Initialized");

        return Error::UNKNOW_FAIL;
    }

    this->_protocol.reset(new TitaniumProtocol());
    this->_transmit_proto.reset(new CommunicationProtobuf());
    this->_receive_proto.reset(new CommunicationProtobuf());

    if (this->_driver.get() == nullptr) {
        return Error::UNKNOW_FAIL;
    }

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
    auto result = Error::UNKNOW_FAIL;

    if ((address == this->_address) ||
        (address == ProtocolConstants::BROADCAST_ADDRESS)) {
        result = ESP_OK;
    }

    return result;
}

bool CommunicationProcess::HasTransmissionPending(void) {
    return this->_shared_memory_manager->IsAreaDataUpdated(ProtobufIndex::LORA_TRANSMIT);
}

bool CommunicationProcess::HasReceivedBytes(void) {
    this->_received_bytes = this->_driver.get()->Read(this->_buffer.get());
    return this->_received_bytes > 0;
}

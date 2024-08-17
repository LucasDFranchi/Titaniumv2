#include "SystemProcess/CommunicationProcess/inc/CommunicationProcess.h"
#include "HAL/memory/SharedMemoryManager.h"

#include "esp_log.h"
#include "esp_timer.h"

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
        case State::READ:
            next_state = this->Read();
            break;
        case State::SINGLE:
            next_state = this->Single();
            break;
        case State::CONTINUOS:
            next_state = this->Continuos();
            break;

        default:
            next_state = State::IDLE;
            break;
    }

    if (next_state != this->communication_state) {
        // ESP_LOGI("Communication Process", "State transition from %d to %d",
        //          static_cast<int>(this->communication_state),
        //          static_cast<int>(next_state));
        this->communication_state = next_state;
    }
}

CommunicationProcess::State CommunicationProcess::Idle(void) {
    auto next_state = this->communication_state;

    if (this->IsReadyToRead()) {
        next_state = State::READ;
    } else if (this->IsReadyToTransmitSingle()) {
        next_state = State::SINGLE;
    } else {
        next_state = State::CONTINUOS;
    }

    return next_state;
}

CommunicationProcess::State CommunicationProcess::Read(void) {
    std::unique_ptr<TitaniumPackage> package = nullptr;
    auto result                              = this->_protocol->Decode(this->_buffer_in,
                                                                       this->_received_bytes,
                                                                       package);
    if (result == ESP_OK) {
        if (!this->CheckAddressPackage(package.get()->address())) {
            this->ProcessReceivedPackage(package);
        } else {
            /* Case in the future we support Daisy chain we need to
             * update this to a forwarding state.
             */
            ESP_LOGI("Communication Process", "Forwarding");
        }
        this->_received_bytes = 0;
    } else {
        ESP_LOGE("Communication Process", "Decode Error: %d", (int)result);
    }

    return State::IDLE;
}

titan_err_t CommunicationProcess::ProcessReceivedPackage(std::unique_ptr<TitaniumPackage>& package) {
    char response_buffer[256] = {0};  // Add something in the proto to get the string maximum size;
    auto result               = Error::UNKNOW_FAIL;

    do {
        auto received_bytes = package.get()->Consume(reinterpret_cast<uint8_t*>(response_buffer));
        if (received_bytes == 0) {
            result = Error::CONSUME_ERROR;
            break;
        }

        result = this->_shared_memory_manager->Write(package.get()->memory_area(),
                                                     response_buffer,
                                                     received_bytes);

        if (result != ESP_OK) {
            result = Error::DESERIALIZE_ERROR;
            break;
        }

    } while (0);

    return result;
}

CommunicationProcess::State CommunicationProcess::Single(void) {
    char response_buffer[256] = {0};  // Add something in the proto to get the string maximum size;
    packet_request_t packet_request{};

    this->_shared_memory_manager->Read(this->_single_packet,
                                       packet_request,
                                       packet_request_t_msg);

    auto read_bytes = this->_shared_memory_manager->Read(packet_request.requested_area,
                                                         response_buffer,
                                                         sizeof(response_buffer));

    auto packet = std::make_unique<TitaniumPackage>(
        read_bytes,
        packet_request.destination_address,
        packet_request.destination_area,
        response_buffer);

    return State::IDLE;
}

CommunicationProcess::State CommunicationProcess::Continuos(void) {
    char response_buffer[256] = {0};  // Add something in the proto to get the string maximum size;

    if (this->_shared_memory_manager->IsAreaDataUpdated(this->_continuos_packet)) {
        ESP_LOGI("Communication Process", "Continuos Packet Configuration Updated");

        auto read_bytes = this->_shared_memory_manager->Read(this->_continuos_packet,
                                                             this->_cp_list,
                                                             continuos_packet_list_t_msg);

        if (read_bytes == 0) {
            ESP_LOGE("Communication Process", "Couldn't read the config area!", );
        }
    }

    uint64_t current_time = esp_timer_get_time();

    for (uint8_t i = 0; i < this->_cp_list.packet_configs_count; i++) {
        if ((current_time - this->_cp_list.packet_configs[i].last_transmission) >
            this->_cp_list.packet_configs[i].packet_interval) {

            auto read_bytes = this->_shared_memory_manager->Read(this->_cp_list.packet_configs[i].requested_area,
                                                                 response_buffer,
                                                                 sizeof(response_buffer));

            auto packet = std::make_unique<TitaniumPackage>(
                read_bytes,
                this->_cp_list.packet_configs[i].destination_address,
                this->_cp_list.packet_configs[i].destination_area,
                response_buffer);

            auto response_buffer_size = this->_protocol->Encode(packet, this->_buffer_out, this->_driver->buffer_size());
            this->_driver->Write(this->_buffer_out, response_buffer_size);

            this->_cp_list.packet_configs[i].last_transmission = current_time;
        }
    }

    return State::IDLE;
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

    this->_protocol = new TitaniumProtocol();

    if (this->_driver == nullptr) {
        return Error::UNKNOW_FAIL;
    }

    return Error::NO_ERROR;
}

titan_err_t CommunicationProcess::InstallDriver(IDriverInterface* driver_interface,
                                                memory_areas_t single_packet,
                                                memory_areas_t continuos_packet) {
    if (driver_interface == nullptr) {
        return Error::UNKNOW_FAIL;
    }

    this->_driver           = driver_interface;
    this->_buffer_in        = new uint8_t[driver_interface->buffer_size()];
    this->_buffer_out       = new uint8_t[driver_interface->buffer_size()];
    this->_single_packet    = single_packet;
    this->_continuos_packet = continuos_packet;

    memset_s(this->_buffer_in, 0, driver_interface->buffer_size());
    memset_s(this->_buffer_out, 0, driver_interface->buffer_size());

    return ESP_OK;
}

void CommunicationProcess::Configure(uint16_t address) {
    this->_address = address;
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

bool CommunicationProcess::IsReadyToRead(void) {
    this->_received_bytes = this->_driver->Read(this->_buffer_in);
    return this->_received_bytes > 0;
}

bool CommunicationProcess::IsReadyToTransmitSingle(void) {
    return this->_shared_memory_manager->IsAreaDataUpdated(this->_single_packet);
}
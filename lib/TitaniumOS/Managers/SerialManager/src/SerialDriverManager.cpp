#include "SerialDriverManager.h"

#include "Protocols/Titanium/TitaniumProtocol.h"
#include "HAL/memory/SharedMemoryManager.h"

esp_err_t SerialDriverManager::StorePackage(std::unique_ptr<TitaniumPackage>& package) {
    auto payload = std::make_unique<uint8_t[]>(package.get()->size());

    package.get()->Consume(payload.get());

    return shared_memory_manager->Write(package.get()->memory_area(), package.get()->size(), payload.get());
}
esp_err_t SerialDriverManager::RetrievePackage(std::unique_ptr<TitaniumPackage>& package) {
}

void SerialDriverManager::Execute(void) {
    auto shared_memory_manager = SharedMemoryManager::GetInstance();
    auto protocol              = TitaniumProtocol();

    //     uint8_t ACK_OK[5] = {0x41, 0x43, 0x4B, 0x4F, 0x4B};
    //     uint8_t ACK_NOK[6] = {0x41, 0x43, 0x4B, 0x4E, 0x4F, 0x4B};

    if (this->Initialize() != ESP_OK) {
        vTaskDelete(this->process_handler);
    }

    while (1) {
        auto len = uart_read_bytes(UART_NUM_0, this->buffer,
                                   sizeof(this->buffer), pdMS_TO_TICKS(200));
        if (len > 0) {
            std::unique_ptr<TitaniumPackage> package = nullptr;

            auto result = protocol.Decode(this->buffer, len, package);

            if (result != ESP_OK) {
                continue;
            }

            if (package.get()->command() == READ_OPERATION) {
                // Follow Request

            } else if (package.get()->command() == WRITE_OPERATION) {
                result = StorePackage(package);

            }
            // This should be a function
            // if (result == ESP_OK) {
            //     uart_write_bytes(UART_NUM_0, ACK_OK, sizeof(ACK_OK));
            // } else {
            //     uart_write_bytes(UART_NUM_0, ACK_NOK, sizeof(ACK_NOK));
            // }
            // This should be a function
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }



    //                     case READ_OPERATION: {
    //                         uint16_t data_size = memory_manager->Read(
    //                             static_cast<area_index_e>(
    //                                 message_incoming.memory_area),
    //                             message_incoming.pointer_data);

    //                         // uint16_t response_message_size =
    //                         // serial_protocol.GenerateResponseMessage(message_incoming.pointer_data,
    //                         // data_size, message_incoming.memory_area); auto
    //                         // message_sending =
    //                         // serial_protocol.GetMessageSending();

    //                         uart_write_bytes(UART_NUM_0,
    //                                          message_incoming.pointer_data,
    //                                          data_size);
    //                     } break;

    //                 }
    //             }


    //         }

    //         vTaskDelay(pdMS_TO_TICKS(50));
    // }
}

/**
 * Initializes the SerialDriverManager.
 *
 * @returns The result of the initialization process.
 */
esp_err_t SerialDriverManager::Initialize(void) {
    esp_err_t result = ESP_OK;

    this->uart_config.baud_rate           = Baudrate::BaudRate115200;
    this->uart_config.data_bits           = UART_DATA_8_BITS;
    this->uart_config.parity              = UART_PARITY_DISABLE;
    this->uart_config.stop_bits           = UART_STOP_BITS_1;
    this->uart_config.flow_ctrl           = UART_HW_FLOWCTRL_DISABLE;
    this->uart_config.rx_flow_ctrl_thresh = 0;
    this->uart_config.source_clk          = UART_SCLK_APB;

    result += uart_param_config(UART_NUM_0, &this->uart_config);
    result += uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                           UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    result +=
        uart_driver_install(UART_NUM_0, sizeof(this->buffer), 0, 0, NULL, 0);

    uart_flush(UART_NUM_0);

    return result;
}
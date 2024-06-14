// #include "LoRaManager.h"

// #include "esp_log.h"
// #include "gpio/GPIOManager.h"

// static const char* TAG = "LoRa Manager";

// /**
//  * @brief Initializes the LoRaManager.
//  *
//  * This function initializes the MemoryManager, creates an instance of
//  * LoRaDriver, and configures the LoRa transceiver with specific settings.
//  *
//  * @return ESP_OK if initialization is successful, otherwise an error code.
//  */
// esp_err_t LoRaManager::Initialize_(void) {
//     esp_err_t result = ESP_OK;

//     this->memory_manager = MemoryManager::GetInstance();
//     this->lora_driver = new LoRaDriver();
//     result += this->lora_driver->Initialize();
//     this->lora_driver->SetFrequency(Regions::BRAZIL);
//     this->lora_driver->SetCRCMode(CRCMode::ENABLE);
//     // this->lora_driver->SetImplicitHeaderMode(64);

//     return result;
// }

// /**
//  * @brief Executes the LoRaManager functionality in an infinite loop.
//  *
//  * This function runs an infinite loop with a delay of 100 milliseconds. It
//  * checks if there is new data in the LORA_WRITE_AREA of the MemoryManager. If
//  * there is new data, it reads the data, sends a packet using the LoRa
//  * transceiver, and sets the transceiver to receiver mode. If there is data
//  * received, it reads the received data, and writes it to the LORA_READ_AREA of
//  * the MemoryManager.
//  */
// void LoRaManager::Execute(void) {
//     if (this->Initialize_() != ESP_OK) {
//         vTaskDelete(this->process_handler_);
//     }

//     while (1) {
//         if (memory_manager->IsAreaDataNew(LORA_WRITE_AREA)) {
//             uint16_t area_size =
//                 memory_manager->Read(LORA_WRITE_AREA, &this->lora_write_area_);
//             if (area_size > 0) {
//                 this->lora_driver->SendPacket(this->lora_write_area_.tx_buffer,
//                                               area_size);
//             }
//         }
//         this->lora_driver->SetReceiverMode();
//         if (this->lora_driver->isDataInReceiver()) {
//             uint8_t received_bytes = this->lora_driver->ReceivePacket(
//                 this->lora_read_area_.rx_buffer,
//                 sizeof(this->lora_read_area_.rx_buffer));
//             if (this->memory_manager->Write(LORA_READ_AREA, received_bytes,
//                                             &this->lora_read_area_) == ESP_OK) {
//                 ESP_LOGI(TAG, "Received Bytes: %d", received_bytes);
//                 ESP_LOGI(TAG, "Data: %s", this->lora_read_area_.rx_buffer);
//             }
//         }

//         vTaskDelay(pdMS_TO_TICKS(100));
//     }
// }
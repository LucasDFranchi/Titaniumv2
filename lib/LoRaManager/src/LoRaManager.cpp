#include "LoRaManager.h"

esp_err_t LoRaManager::Initialize_(void){
    esp_err_t result = ESP_OK;

    this->memory_manager = MemoryManager::GetInstance();
    this->lora_driver = new LoRaDriver();
    result += this->lora_driver->Initialize();
    this->lora_driver->SetFrequency(915e6); // Change to namespace REGIONS::BRAZIL = 915e6
    this->lora_driver->SetCRCMode(CRCMode::ENABLE);
    this->lora_driver->SetImplicitHeaderMode(64);

    ESP_ERROR_CHECK(result);

    return result;
}

/**
 * The function `Execute` runs an infinite loop with a delay of 100 milliseconds.
 */
void LoRaManager::Execute(void){

    this->Initialize_();

    while(1){
        if (memory_manager->IsAreaDataNew(LORA_WRITE_AREA)){
            uint16_t area_size = 0;
            memory_manager->Read(LORA_WRITE_AREA, &area_size, this->tx_buffer_);
            this->lora_driver->SendPacket(this->tx_buffer_, area_size);
        }
        this->lora_driver->SetReceiverMode();
        if (this->lora_driver->isDataInReceiver()){
            uint8_t recieved_bytes = this->lora_driver->ReceivePacket(this->rx_buffer_, sizeof(this->rx_buffer_));
            this->memory_manager->Write(LORA_READ_AREA, recieved_bytes, this->rx_buffer_);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
#include "MemoryAreaLoRaWrite.h"

/**
 * Initializes the MemoryAreaLoRaWrite object.
 *
 * @returns ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t MemoryAreaLoRaWrite::Initialize(void){

    this->index = LORA_WRITE_AREA;
    this->data = new uint8_t[64];
    this->size = 64;
    this->access_type = READ_WRITE;
    this->cached_time = 0;
    this->has_update = false;
    this->mutex_ = xSemaphoreCreateMutex();
    memset_s(this->data, 0, this->size);

    return ESP_OK;
}
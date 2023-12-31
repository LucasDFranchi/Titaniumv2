#include "../MemoryAreaTemplate.h"

#ifndef MEMORY_LORA_WRITE_AREA_GUARD
#define MEMORY_LORA_WRITE_AREA_GUARD    

class MemoryAreaLoRaWrite: public MemoryAreaTemplate {
public:
    esp_err_t Initialize(void) override {
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
};

#endif /* MEMORY_LORA_WRITE_AREA_GUARD  */
#include "../MemoryAreaTemplate.h"

#ifndef MEMORY_AREA_LORA_READ_GUARD
#define MEMORY_AREA_LORA_READ_GUARD

class MemoryAreaLoRaRead: public MemoryAreaTemplate{
    public:
    esp_err_t Initialize(void) override;
};

#endif /* MEMORY_AREA_LORA_READ_GUARD */
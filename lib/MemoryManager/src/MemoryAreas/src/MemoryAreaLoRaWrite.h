#include "../MemoryAreaTemplate.h"

#ifndef MEMORY_AREA_LORA_WRITE_GUARD
#define MEMORY_AREA_LORA_WRITE_GUARD

class MemoryAreaLoRaWrite: public MemoryAreaTemplate{
    public:
    esp_err_t Initialize(void) override;
};

#endif /* MEMORY_AREA_LORA_WRITE_GUARD */
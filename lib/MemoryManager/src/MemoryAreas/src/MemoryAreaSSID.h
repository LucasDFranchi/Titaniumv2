#include "../MemoryAreaTemplate.h"

#ifndef MEMORY_AREA_SSID_GUARD
#define MEMORY_AREA_SSID_GUARD

class MemoryAreaSSID: public MemoryAreaTemplate{
    public:
    esp_err_t Initialize(void) override;
};

#endif /* MEMORY_AREA_SSID_GUARD */
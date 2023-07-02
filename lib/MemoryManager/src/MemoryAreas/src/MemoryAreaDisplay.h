#include "../IMemoryArea.h"

#ifndef MEMORY_AREA_DISPLAY_GUARD
#define MEMORY_AREA_DISPLAY_GUARD

// This needs to be a template!!!!

class MemoryAreaDisplay: public IMemoryArea{
    public:
    esp_err_t Initialize(void);
    esp_err_t Clear(void);
    uint32_t GetSize(void);
    access_type_e GetAccess(void);
    area_index_e GetAreaIndex(void);
    esp_err_t Write(uint8_t *pIn);
    esp_err_t Read(uint8_t *pOut);
    private:
    area_index_e index;
    access_type_e access_type;
    uint8_t data[1024];
    uint8_t has_update;
    uint32_t size;
    uint32_t cached_time;
    SemaphoreHandle_t mutex_ = nullptr;
};

#endif /* MEMORY_AREA_DISPLAY_GUARD */
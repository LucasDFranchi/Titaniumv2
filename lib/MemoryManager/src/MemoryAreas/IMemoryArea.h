#include "esp_err.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/semphr.h"

#include "MemoryAreaEnum.h"

#ifndef MEMORY_AREA_INTERFACE_GUARD
#define MEMORY_AREA_INTERFACE_GUARD

/**
 * Interface for a memory area.
 */
class IMemoryArea {
    public:
    virtual esp_err_t Initialize(void) = 0;
    virtual esp_err_t Clear(void) = 0;
    virtual uint32_t GetSize(void) = 0;
    virtual access_type_e GetAccess(void) = 0;
    virtual area_index_e GetAreaIndex(void) = 0;
    virtual esp_err_t Write(uint8_t *pIn) = 0;
    virtual esp_err_t Read(uint8_t *pOut) = 0;
};

#endif /* MEMORY_AREA_INTERFACE_GUARD */
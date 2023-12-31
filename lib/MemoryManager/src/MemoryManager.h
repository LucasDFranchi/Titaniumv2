#include "stdint.h"
#include "esp_err.h"
#include "./MemoryAreas/MemoryAreaTemplate.h"

#ifndef MEMORY_MANAGER_GUARD
#define MEMORY_MANAGER_GUARD

/**
 * Manages memory allocation and access for a system.
 */
class MemoryManager{
    public:
    MemoryManager(const MemoryManager& obj) = delete;

    esp_err_t Initialize(void);
    esp_err_t Write(area_index_e area_index, uint32_t size, uint8_t *pIn);
    esp_err_t Read(area_index_e area_index, uint16_t *size_pointer, uint8_t *pOut);
    bool      IsAreaDataNew(area_index_e area_index);
    static MemoryManager* GetInstance(void);
    private:
    MemoryManager(){};
    uint8_t memory_area_array_size;
    MemoryAreaTemplate* memory_area_array[AREAS_COUNT];
    
    static MemoryManager* singleton_pointer_;
};

#endif /* MEMORY_MANAGER_GUARD */
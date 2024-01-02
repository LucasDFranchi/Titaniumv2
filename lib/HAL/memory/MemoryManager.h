#include "stdint.h"
#include "esp_err.h"
#include "./MemoryAreas/MemoryArea.hpp"

#ifndef MEMORY_MANAGER_GUARD
#define MEMORY_MANAGER_GUARD

/**
 * Manages memory allocation and access for a system.
 */
class MemoryManager{
    public:
    MemoryManager(const MemoryManager& obj) = delete;

    esp_err_t Initialize(void);
    esp_err_t Write(area_index_e area_index, uint16_t size, IArea &area_data);
    esp_err_t Write(area_index_e area_index, uint16_t size, uint8_t* data_pointer);
    uint16_t  Read(area_index_e area_index, IArea &area_data);
    uint16_t  Read(area_index_e area_index, uint8_t* pointer_data);
    bool      IsAreaDataNew(area_index_e area_index);
    static MemoryManager* GetInstance(void);
    private:
    MemoryManager(){};
    uint8_t memory_area_array_size;
    MemoryArea* memory_area_array[AREAS_COUNT];
    
    static MemoryManager* singleton_pointer_;
};

#endif /* MEMORY_MANAGER_GUARD */
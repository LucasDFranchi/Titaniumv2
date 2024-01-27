#include "MemoryManager.hpp"

#include "memory/AreaDefinitions/ConnectionArea.h"
#include "memory/AreaDefinitions/CredentialsArea.h"
#include "memory/AreaDefinitions/DisplayArea.h"
#include "memory/AreaDefinitions/LoRaReadArea.h"
#include "memory/AreaDefinitions/LoRaWriteArea.h"

constexpr AreaDef memory_area_def[AREAS_COUNT]{
    {READ_WRITE, DISPLAY_AREA, sizeof(display_st), 0},
    {READ_WRITE, CONNECTION_AREA, sizeof(connection_st), 0},
    {READ_WRITE, LORA_READ_AREA, sizeof(lora_read_st), 0},
    {READ_WRITE, LORA_WRITE_AREA, sizeof(lora_write_st), 0},
    {READ_WRITE, CREDENTIALS_AREA, sizeof(credentials_st), 0},
};

/**
 * Initializes the MemoryManager.
 *
 * @returns ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t MemoryManager::Initialize(void) {
    for (int i = 0; i < AREAS_COUNT; i++) {
        this->memory_area_array[i] = new MemoryArea(memory_area_def[i]);
    }

    return ESP_OK;
}

bool MemoryManager::IsAreaDataNew(area_index_e area_index) {
    return this->memory_area_array[area_index]->GetAreaHasUpdated();
}

/**
 * @brief Returns the singleton instance
 *
 * @return MemoryManager*
 */
MemoryManager* MemoryManager::GetInstance(void) {
    if (singleton_pointer_ == nullptr) {
        singleton_pointer_ = new MemoryManager();
    }

    return singleton_pointer_;
}
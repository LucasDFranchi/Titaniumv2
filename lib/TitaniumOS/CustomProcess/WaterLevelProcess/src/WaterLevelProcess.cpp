#include "WaterLevelProcess.h"

#include "HAL/memory/MemoryHandlers.h"
#include "SystemProcess/ProcessAreasIndex.h"

#include "./../proto/WaterLevel.pb-c.h"

#include "esp_log.h"
#include <string.h>


void WaterLevelProcess::Execute(void) {
    if (this->Initialize() != ESP_OK) {
        vTaskDelete(this->_process_handler);
    }

    WaterLevel wl = WATER_LEVEL__INIT;

    const char* foo = "FIELD1";
    const char* bar = "FIELD1";

    memcpy(wl.field1, foo, strlen(foo));
    memcpy(wl.field2, bar, strlen(bar));

    ESP_LOGI("WATER_LEVEL_SENSOR", "message size: %d", water_level__get_packed_size(&wl));

    // water_level_st water_level{0};

    while (1) {
        // water_level.value = 0xBAAD;
        // water_level.timestamp = 0xDEADBEEF;

        // this->_shared_memory_manager.get()->Write(CustomProcessAreaIndex::WATER_LEVEL, sizeof(water_level_st), &water_level);

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

/**
 * Initializes the CommunicationManager.
 *
 * @returns The result of the initialization process.
 */
esp_err_t WaterLevelProcess::Initialize(void) {
    this->_shared_memory_manager.reset(SharedMemoryManager::GetInstance());
    return ESP_OK;
}
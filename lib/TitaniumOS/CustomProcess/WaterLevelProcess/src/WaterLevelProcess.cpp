#include "./../inc/WaterLevelProcess.h"
#include "./../inc/WaterLevelProto.h"

#include "HAL/memory/MemoryHandlers.h"
#include "SystemProcess/ProcessAreasIndex.h"

#include "esp_timer.h"
#include "esp_log.h"
#include <string.h>


void WaterLevelProcess::Execute(void) {
    if (this->Initialize() != ESP_OK) {
        vTaskDelete(this->_process_handler);
    }

    WaterLevelProtobuf water_level_proto{};
    uint32_t counter = 0;

    while (1) {
        water_level_proto.UpdateTimestamp(esp_timer_get_time());
        water_level_proto.UpdateValue(counter);
        counter += 1;

        this->_shared_memory_manager.get()->Write(CustomProcessAreaIndex::WATER_LEVEL, &water_level_proto);

        vTaskDelay(pdMS_TO_TICKS(1000));
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
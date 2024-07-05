#include "./../inc/WaterLevelProcess.h"
#include "./../inc/WaterLevelProto.h"

#include "HAL/memory/MemoryHandlers.h"
#include "SystemProcess/ProcessAreasIndex.h"

#include "esp_log.h"
#include <string.h>


void WaterLevelProcess::Execute(void) {
    if (this->Initialize() != ESP_OK) {
        vTaskDelete(this->_process_handler);
    }

    WaterLevelProtobuf water_level_pb{};

    // water_level_st water_level{0};

    while (1) {
        water_level_pb.UpdateTimestamp(123456);
        water_level_pb.UpdateValue(123);

        this->_shared_memory_manager.get()->Write(CustomProcessAreaIndex::WATER_LEVEL, &water_level_pb);

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
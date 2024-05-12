#include "WaterLevelProcess.h"

#include "HAL/memory/MemoryHandlers.h"
#include "Managers/ManagersAreaIndex.h"

const char* arr[] = {
    "2.91M", "3.14M", "0.12M", "1.76M", "0.98M",
    "3.82M", "2.45M", "0.55M", "1.23M", "3.67M",
    "0.89M", "2.34M", "1.45M", "3.01M", "0.76M",
    "3.22M", "0.43M", "2.87M", "1.67M", "0.21M",
    "2.09M", "1.98M", "0.34M", "3.55M", "0.65M",
    "2.76M", "1.12M", "0.99M", "3.44M", "1.89M"};

void WaterLevelProcess::Execute(void) {
    if (this->Initialize() != ESP_OK) {
        vTaskDelete(this->_process_handler);
    }

    uint8_t cont = 0;

    while (1) {
        this->_shared_memory_manager.get()->Write(ManagersAreaIndex::UART_TRANSMIT, sizeof(water_level_st), const_cast<char*>(arr[cont]));

        if (cont > sizeof(arr)) {
            cont = 0;
        } else {
            cont++;
        }

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

/**
 * Initializes the SerialCommunicationManager.
 *
 * @returns The result of the initialization process.
 */
esp_err_t WaterLevelProcess::Initialize(void) {
    this->_shared_memory_manager.reset(SharedMemoryManager::GetInstance());
    return ESP_OK;
}
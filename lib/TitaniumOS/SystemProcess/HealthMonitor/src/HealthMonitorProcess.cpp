#include "SystemProcess/HealthMonitor/inc/HealthMonitorProcess.h"

#include "esp_log.h"
#include "string.h"

static const char* TAG = "Health Monitor";

esp_err_t HealthMonitorProcess::Initialize(void) {
    auto result = ESP_OK;

    this->_shared_memory_manager.reset(SharedMemoryManager::GetInstance());

    return result;
}

/**
 * @brief Main execution loop for HTTPServerProcess.
 *
 * This function runs an infinite loop with a delay of 1000 milliseconds.
 */
void HealthMonitorProcess::Execute(void) {
    if (this->Initialize() != ESP_OK) {
        vTaskDelete(this->_process_handler);
    }

    char log_buffer[1024];
    char temp_buffer[124];

    while (1) {
        log_buffer[0] = '\0';

        for (uint8_t i = 0; i < this->_MAXIMUM_PROCESS_MONITORING; i++) {
            if (this->_process_handlers_list[i] != nullptr) {
                auto process = this->_process_handlers_list[i];

                snprintf(temp_buffer, sizeof(temp_buffer), "%s: %lu|", process->GetTaskName(), process->GetTaskHeapUsage());
                strncat(log_buffer, temp_buffer, sizeof(log_buffer) - strlen(log_buffer) - 1);
            }
        }
        ESP_LOGI(TAG, "%s",log_buffer);

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

esp_err_t HealthMonitorProcess::MonitorProcess(ProcessTemplate* process) {
    auto result = ESP_FAIL;

    for (uint8_t i = 0; i < this->_MAXIMUM_PROCESS_MONITORING; i++) {
        if (this->_process_handlers_list[i] == nullptr) {
            this->_process_handlers_list[i] = process;

            result = ESP_OK;
            break;
        }
    }
    return result;
}
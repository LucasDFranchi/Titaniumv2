#ifndef HEALTH_MONITOR_PROCESS_H
#define HEALTH_MONITOR_PROCESS_H

#include "HAL/memory/SharedMemoryManager.h"
#include "SystemProcess/Template/ProcessTemplate.h"

#include "esp_err.h"

namespace HealthMonitor {
    constexpr bool ENABLE_MONITORING = true;
}

class HealthMonitorProcess : public ProcessTemplate {
   public:
    /**
     * @brief Constructor for HealthMonitorProcess class.
     *
     * @param[in] name Name of the HTTP server manager process.
     * @param[in] memory Memory allocated for the process.
     * @param[in] priority Priority of the process.
     */
    HealthMonitorProcess(const char* name, uint32_t memory, UBaseType_t priority)
        : ProcessTemplate(name, memory, priority, &this->_process_handler) {}

   public:
    esp_err_t MonitorProcess(ProcessTemplate* process_handler);

   private:
    void Execute(void);
    esp_err_t Initialize(void);

   private:
    constexpr static uint8_t _MAXIMUM_PROCESS_MONITORING        = 32;
    TaskHandle_t _process_handler                               = nullptr; /**< Handle for the HTTP server process task. */
    ProcessTemplate* _process_handlers_list[32]                 = {0};     // TODO: link to the variable
    std::unique_ptr<SharedMemoryManager> _shared_memory_manager = nullptr;
};

#endif /* HEALTH_MONITOR_PROCESS_H */
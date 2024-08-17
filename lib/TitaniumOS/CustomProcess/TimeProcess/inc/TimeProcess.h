#ifndef TIME_PROCESS_H
#define TIME_PROCESS_H

#include "HAL/memory/SharedMemoryManager.h"
#include "Protocols/Protobuf/inc/titanium.pb.h"
#include "SystemProcess/Template/ProcessTemplate.h"
#include "esp_err.h"

/**
 * @brief Class that manages time processing functionality.
 *
 * This class is responsible for managing time-related processes, including
 * initialization and execution of time-sensitive tasks. It inherits from
 * ProcessTemplate to provide a structure for processing tasks in a
 * real-time operating environment.
 */
class TimeProcess : public ProcessTemplate {
   public:
    /**
     * @brief Constructor for the TimeProcess class.
     *
     * Initializes a new instance of the TimeProcess class with the specified
     * parameters.
     *
     * @param[in] name Name of the time processing manager.
     * @param[in] memory Memory allocated for the process in bytes.
     * @param[in] priority Priority level of the process for scheduling.
     */
    TimeProcess(const char* name, uint32_t memory, UBaseType_t priority)
        : ProcessTemplate(name, memory, priority, &this->_process_handler) {}

   private:
    void Execute(void);
    titan_err_t Initialize(void);

   private:
    TaskHandle_t _process_handler               = nullptr; /**< Handle for the time processing task. */
    SharedMemoryManager* _shared_memory_manager = nullptr; /**< Pointer to the shared memory manager instance used by this process. */
    time_process_t time_process{};
};

#endif /* TIME_PROCESS_H */

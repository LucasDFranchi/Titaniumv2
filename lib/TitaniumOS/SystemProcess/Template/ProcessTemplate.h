
#ifndef PROCESS_TEMPLATE_H
#define PROCESS_TEMPLATE_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "stdio.h"

/**
 * A base class for creating and executing processes.
 */
class ProcessTemplate {
   public:
    /**
     * @brief Constructor for the ProcessTemplate class.
     *
     * @param[in] object_pointer Pointer to the process object.
     * @param[in] name Name of the process.
     * @param[in] memory Memory size allocated for the process.
     * @param[in] priority Priority of the process.
     * @param[in] handle Handle for the task.
     */
    // ProcessTemplate(ProcessTemplate *object_pointer, const char *name, uint32_t memory, UBaseType_t priority, TaskHandle_t *handle) {
    ProcessTemplate(const char *name, uint32_t memory, UBaseType_t priority, TaskHandle_t *handle) {
        this->object_pointer = this;
        this->name           = name;
        this->memory         = memory;
        this->priority       = priority;
        this->handle         = handle;
    }

    /**
     * @brief Static method to execute the process.
     *
     * @param pvParameters Pointer to the process parameters.
     */
    static void ExecuteProcess(void *pvParameters) {
        ProcessTemplate *process_pointer_object = static_cast<ProcessTemplate *>(pvParameters);
        process_pointer_object->Execute();
    }

    /**
     * @brief Initializes the process by creating a FreeRTOS task.
     */
    void InitializeProcess() {
        xTaskCreate(ProcessTemplate::ExecuteProcess, this->name, this->memory, static_cast<void *>(this->object_pointer), this->priority, this->handle);
    }

    TaskHandle_t task_handle(void) const {
        return *this->handle;
    }

   private:
    /**
     * @brief Pure virtual function to be implemented by derived classes to define process execution logic.
     */
    virtual void Execute(void) = 0;

   private:
    const char *name = "Generic Task"; /**< Name of the process. */
    ProcessTemplate *object_pointer;   /**< Pointer to the process object. */
    uint32_t memory      = 0;          /**< Memory size allocated for the process. */
    UBaseType_t priority = 0;          /**< Priority of the process. */
    TaskHandle_t *handle = NULL;       /**< Handle for the task. */
};

#endif /* PROCESS_TEMPLATE_H */
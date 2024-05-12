#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "stdio.h"

#ifndef PROCESS_TEMPLATE_GUARD
#define PROCESS_TEMPLATE_GUARD

/**
 * A base class for creating and executing processes.
 */
class ProcessTemplate{
    public:
    ProcessTemplate (ProcessTemplate *object_pointer, const char* name, uint32_t memory, UBaseType_t  priority, TaskHandle_t *handle){
      this->object_pointer = object_pointer;
      this->name = name;
      this->memory = memory;
      this->priority = priority;
      this->handle = handle;
    }
    static void ExecuteProcess(void *pvParameters ){
      ProcessTemplate * process_pointer_object = static_cast<ProcessTemplate *>(pvParameters);
      process_pointer_object->Execute();
    }
    void InitializeProcess(){
      xTaskCreate(ProcessTemplate::ExecuteProcess, this->name, this->memory, static_cast<void *>(this->object_pointer), this->priority, this->handle);
    }
    virtual void Execute(void) = 0;
    private:
    const char* name = "Generic Task";
    ProcessTemplate *object_pointer;
    uint32_t memory = 0;
    UBaseType_t priority = 0;
    TaskHandle_t *handle = NULL;
};

#endif /* PROCESS_TEMPLATE_GUARD */
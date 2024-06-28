#ifndef WATER_LEVEL_PROCESS
#define WATER_LEVEL_PROCESS

#include "HAL/memory/SharedMemoryManager.h"
#include "SystemProcess/Template/ProcessTemplate.h"

typedef struct water_level_st {
    uint8_t water_level[6];
} __attribute__((packed)) water_level_st;

class WaterLevelProcess : public ProcessTemplate {
   public:
    WaterLevelProcess(const char* name, uint32_t memory, UBaseType_t priority)
        : ProcessTemplate(name, memory, priority, &this->_process_handler){};
    void Execute(void);

   private:
    esp_err_t Initialize(void);

   private:
    TaskHandle_t _process_handler                               = NULL;
    std::unique_ptr<SharedMemoryManager> _shared_memory_manager = nullptr;

   private:
   uint8_t _water_level[5] = {0};
};

#endif /* WATER_LEVEL_PROCESS */
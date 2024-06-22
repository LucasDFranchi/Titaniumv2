#ifndef WATER_LEVEL_PROCESS
#define WATER_LEVEL_PROCESS

#include "HAL/memory/SharedMemoryManager.h"
#include "Process/Template/ProcessTemplate.h"
#include "HAL/gpio/GPIOManager.h"

#define WATER_LEVEL_TIMEOUT 100000

typedef struct water_level_st {
    uint8_t water_level[5];
} __attribute__((packed)) water_level_st;

class WaterLevelProcess : public ProcessTemplate {
   public:
    WaterLevelProcess(const char* name, uint32_t memory, UBaseType_t priority)
        : ProcessTemplate(this, name, memory, priority, &this->_process_handler){};
    void Execute(void);

   private:
    esp_err_t Initialize(void);
    uint32_t GetWaterLevel(void);
    uint32_t CalculateDistance(uint32_t timer_count);
    void UpdateWaterLevel(uint32_t distance);

    uint8_t IncrementAndCheckTimeout();
    void ResetTimeout(void);

   private:
    TaskHandle_t _process_handler                               = NULL;
    std::unique_ptr<SharedMemoryManager> _shared_memory_manager = nullptr;

   private:
    uint32_t _timeout_count = 0;
    uint8_t _water_level[5] = {0};
    uint32_t _distance;
    GPIOManager* _gpio_manager;

   
};



#endif /* WATER_LEVEL_PROCESS */
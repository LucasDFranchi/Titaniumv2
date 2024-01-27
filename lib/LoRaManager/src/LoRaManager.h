#ifndef LORA_MANAGER_GUARD
#define LORA_MANAGER_GUARD

#include "ProcessTemplate.h"
#include "Driver/LoRaDriver.h"
#include "memory/MemoryManager.hpp"
#include "memory/AreaDefinitions/LoRaReadArea.h"
#include "memory/AreaDefinitions/LoRaWriteArea.h"

#include "stdint.h"
#include "esp_err.h"

namespace Regions {
    constexpr uint32_t EUROPE    = 868e6;
    constexpr uint32_t US        = 915e6;
    constexpr uint32_t ASIA      = 920e6;
    constexpr uint32_t AUSTRALIA = 915e6;
    constexpr uint32_t CHINA     = 782e6;
    constexpr uint32_t INDIA     = 866e6;
    constexpr uint32_t KOREA     = 920e6;
    constexpr uint32_t JAPAN     = 920e6;
    constexpr uint32_t BRAZIL    = 915e6;
}

/**
 * Manages LoRa communication.
 */
class LoRaManager : public ProcessTemplate{
    public:
    LoRaManager(const char* name, uint32_t memory, UBaseType_t  priority) : ProcessTemplate(this, name, memory, priority, &this->process_handler_) { };
    void Execute(void);

    private:
    esp_err_t Initialize_(void);

    private:
    TaskHandle_t   process_handler_ = nullptr;
    MemoryManager* memory_manager = nullptr;
    LoRaDriver*    lora_driver = nullptr;
    lora_read_st   lora_read_area_;
    lora_write_st  lora_write_area_;
};

#endif /* LORA_MANAGER_GUARD */
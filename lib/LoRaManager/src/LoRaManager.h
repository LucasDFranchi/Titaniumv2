#ifndef LORA_MANAGER_GUARD
#define LORA_MANAGER_GUARD

#include "stdint.h"
#include "esp_err.h"

#include "ProcessTemplate.h"
#include "MemoryManager.h"
#include "Driver/LoRaDriver.h"

/**
 * Manages LoRa communication.
 */
class LoRaManager : public ProcessTemplate{
    public:
    LoRaManager() : ProcessTemplate(this, "LoRa Proccess", 1024*3, 1, &this->process_handler) { };
    void Execute(void);

    private:
    esp_err_t Initialize_(void);

    private:
    TaskHandle_t   process_handler = NULL;
    MemoryManager* memory_manager = nullptr;
    LoRaDriver*    lora_driver = nullptr;
    uint8_t tx_buffer_[64] = {0};
    uint8_t rx_buffer_[64] = {0};
};

#endif /* LORA_MANAGER_GUARD */
#ifndef COMMUNICATION_MANAGER
#define COMMUNICATION_MANAGER

#include "HAL/memory/SharedMemoryManager.h"
#include "Managers/ManagersAreaIndex.h"
#include "Process/Template/ProcessTemplate.h"
#include "Protocols/Titanium/TitaniumPackage.h"
#include "Drivers/DriverInterface/ICommunicationDriver.h"
#include "Drivers/UART/UARTDriver.h"

#include <memory>

#include "esp_err.h"

/**
 * A class that manages the graphic driver process.
 */
class SerialCommunicationManager : public ProcessTemplate {
   public:
    SerialCommunicationManager(const char* name, uint32_t memory, UBaseType_t priority)
        : ProcessTemplate(this, name, memory, priority, &this->_process_handler){};
    void Execute(void);
    void InstallDriver(IDriverInterface* driver_interface, uint8_t memory_area);

   private:
    esp_err_t Initialize(void);
    esp_err_t StorePackage(std::unique_ptr<TitaniumPackage>& package);
    std::unique_ptr<TitaniumPackage> GenerateResponsePackage(uint8_t memory_area);
    void Acknowledge(esp_err_t result);

   private:
    std::unique_ptr<uint8_t[]> _buffer                          = nullptr;
    TaskHandle_t _process_handler                               = NULL;
    std::unique_ptr<IDriverInterface> _driver                   = nullptr;
    std::unique_ptr<SharedMemoryManager> _shared_memory_manager = nullptr;

   private:
    uint8_t _memory_area = ManagersAreaIndex::INVALID;
};

#endif /* COMMUNICATION_MANAGER */
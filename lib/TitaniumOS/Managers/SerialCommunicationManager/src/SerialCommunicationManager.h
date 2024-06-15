#ifndef COMMUNICATION_MANAGER
#define COMMUNICATION_MANAGER

#include "Process/ProcessTemplate.h"
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
    void InstallDriver(IDriverInterface* driver_interface);

   private:
    esp_err_t Initialize(void);
    esp_err_t StorePackage(std::unique_ptr<TitaniumPackage>& package);
    std::unique_ptr<TitaniumPackage> GenerateResponsePackage(uint8_t memory_area);
    void Acknowledge(esp_err_t result);

   private:
    std::unique_ptr<uint8_t[]> buffer         = nullptr;
    TaskHandle_t _process_handler             = NULL;
    std::unique_ptr<IDriverInterface> _driver = nullptr;
};

#endif /* COMMUNICATION_MANAGER */
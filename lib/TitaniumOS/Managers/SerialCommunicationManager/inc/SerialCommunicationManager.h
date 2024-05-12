#ifndef COMMUNICATION_MANAGER
#define COMMUNICATION_MANAGER

#include "Drivers/DriverInterface/ICommunicationDriver.h"
#include "HAL/memory/SharedMemoryManager.h"
#include "Managers/ManagersAreaIndex.h"
#include "Process/Template/ProcessTemplate.h"
#include "Protocols/Titanium/TitaniumPackage.h"

#include <memory>

#include "esp_err.h"

/**
 * @brief A class that manages the serial communication process.
 */
class SerialCommunicationManager : public ProcessTemplate {
   public:
    /**
     * @brief Constructs a SerialCommunicationManager with the specified parameters.
     *
     * @param name The name of the process.
     * @param memory The memory allocated for the process.
     * @param priority The priority of the process.
     */
    SerialCommunicationManager(const char* name, uint32_t memory, UBaseType_t priority)
        : ProcessTemplate(this, name, memory, priority, &this->_process_handler){};
    void InstallDriver(IDriverInterface* driver_interface, uint8_t memory_area_read, uint8_t memory_area_write);

   private:
    void Execute(void);
    esp_err_t Initialize(void);
    esp_err_t StorePackage(std::unique_ptr<TitaniumPackage>& package);
    std::unique_ptr<TitaniumPackage> GenerateResponsePackage(uint8_t memory_area);
    std::unique_ptr<TitaniumPackage> GenerateTransmissionPackage(void);
    void Acknowledge(esp_err_t result);

   private:
    std::unique_ptr<uint8_t[]> _buffer                          = nullptr;
    TaskHandle_t _process_handler                               = NULL;
    std::unique_ptr<IDriverInterface> _driver                   = nullptr;
    std::unique_ptr<SharedMemoryManager> _shared_memory_manager = nullptr;

   private:
    uint8_t _memory_area_receive  = ManagersAreaIndex::INVALID;
    uint8_t _memory_area_transmit = ManagersAreaIndex::INVALID;
};

#endif /* COMMUNICATION_MANAGER */
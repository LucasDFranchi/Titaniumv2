#ifndef COMMUNICATION_PROCESS_H
#define COMMUNICATION_PROCESS_H

#include "Drivers/DriverInterface/ICommunicationDriver.h"
#include "HAL/memory/SharedMemoryManager.h"
#include "Protocols/Protobuf/inc/ProtobufFactory.h"
#include "Protocols/Titanium/TitaniumPackage.h"
#include "Protocols/Titanium/TitaniumProtocol.h"

#include "SystemProcess/Template/ProcessTemplate.h"

#include <memory>

#include "esp_err.h"

/**
 * @brief A class that manages the serial communication process.
 */
class CommunicationProcess : public ProcessTemplate {
   public:
    /**
     * @brief Constructs a CommunicationProcess with the specified parameters.
     *
     * @param name The name of the process.
     * @param memory The memory allocated for the process.
     * @param priority The priority of the process.
     */
    CommunicationProcess(const char* name, uint32_t memory, UBaseType_t priority)
        : ProcessTemplate(name, memory, priority, &this->_process_handler) {};

    esp_err_t InstallDriver(IDriverInterface* driver_interface);
    void Configure(uint16_t address);

   private:
    void Execute(void);
    esp_err_t Initialize(void);
    esp_err_t StorePackage(std::unique_ptr<TitaniumPackage>& package);
    std::unique_ptr<TitaniumPackage> GenerateResponsePackage(uint8_t memory_area);
    std::unique_ptr<TitaniumPackage> GenerateTransmitPackage(CommunicationProtobuf& communication_proto);
    bool CheckAddressPackage(uint16_t address);
    void Acknowledge(esp_err_t result);
    esp_err_t ProcessReceivedPackage(std::unique_ptr<TitaniumPackage>& package);
    esp_err_t ProcessReadPackage(std::unique_ptr<TitaniumPackage>& package);
    esp_err_t ProcessReadResponsePackage(std::unique_ptr<TitaniumPackage>& package);
    esp_err_t ProcessAckPackage(std::unique_ptr<TitaniumPackage>& package);
    esp_err_t ProcessNackPackage(std::unique_ptr<TitaniumPackage>& package);
    esp_err_t ProcessWritePackage(std::unique_ptr<TitaniumPackage>& package);

   private:
    std::unique_ptr<uint8_t[]> _buffer                          = nullptr;  ///< Buffer for communication.
    TaskHandle_t _process_handler                               = NULL;     ///< Handler for the process task.
    std::unique_ptr<IDriverInterface> _driver                   = nullptr;  ///< Communication driver.
    std::unique_ptr<SharedMemoryManager> _shared_memory_manager = nullptr;  ///< Shared memory manager.
    std::unique_ptr<TitaniumProtocol> _protocol                 = nullptr;  ///< Protocol handler.
    std::unique_ptr<CommunicationProtobuf> _transmit_proto      = nullptr;  ///< Transmission protobuf.
    std::unique_ptr<CommunicationProtobuf> _receive_proto       = nullptr;  ///< Reception protobuf.

   private:
    uint16_t _address = 0xFFFF;  ///< Memory Address of this device
};

#endif /* COMMUNICATION_PROCESS_H */

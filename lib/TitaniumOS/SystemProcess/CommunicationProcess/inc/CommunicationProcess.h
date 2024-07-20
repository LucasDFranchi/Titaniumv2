#ifndef COMMUNICATION_PROCESS_H
#define COMMUNICATION_PROCESS_H

#include "Drivers/DriverInterface/ICommunicationDriver.h"
#include "HAL/memory/SharedMemoryManager.h"
#include "Kernel/error/error_enum.h"
#include "Protocols/Protobuf/inc/ProtobufFactory.h"
#include "Protocols/Titanium/TitaniumPackage.h"
#include "Protocols/Titanium/TitaniumProtocol.h"
#include "SystemProcess/Template/ProcessTemplate.h"

#include <memory>

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

    titan_err_t InstallDriver(IDriverInterface* driver_interface,
                              uint16_t transmit_area,
                              uint16_t received_area);
    void Configure(uint16_t address);

   public:
    uint32_t ack_list[32] = {0};

   private:
    void Execute(void);
    void ProcessState(void);
    titan_err_t Initialize(void);
    titan_err_t StorePackage(std::unique_ptr<TitaniumPackage>& package);
    std::unique_ptr<TitaniumPackage> GenerateResponsePackage(uint8_t memory_area);
    std::unique_ptr<TitaniumPackage> GenerateTransmitPackage(CommunicationProtobuf& communication_proto);
    bool CheckAddressPackage(uint16_t address);
    void Acknowledge(titan_err_t result);
    titan_err_t ProcessReceivedPackage(std::unique_ptr<TitaniumPackage>& package);
    titan_err_t ProcessReadPackage(std::unique_ptr<TitaniumPackage>& package, bool should_ack);
    titan_err_t ProcessReadResponsePackage(std::unique_ptr<TitaniumPackage>& package, bool should_ack);
    titan_err_t ProcessAckPackage(std::unique_ptr<TitaniumPackage>& package);
    titan_err_t ProcessWritePackage(std::unique_ptr<TitaniumPackage>& package, bool should_ack);
    bool HasTransmissionPending(void);
    bool HasReceivedBytes(void);

   private:
    std::unique_ptr<uint8_t[]> _buffer                          = nullptr;  ///< Buffer for communication.
    TaskHandle_t _process_handler                               = nullptr;  ///< Handler for the process task.
    std::unique_ptr<IDriverInterface> _driver                   = nullptr;  ///< Communication driver.
    std::unique_ptr<SharedMemoryManager> _shared_memory_manager = nullptr;  ///< Shared memory manager.
    std::unique_ptr<TitaniumProtocol> _protocol                 = nullptr;  ///< Protocol handler.
    std::unique_ptr<CommunicationProtobuf> _transmit_proto      = nullptr;  ///< Transmission protobuf.
    std::unique_ptr<CommunicationProtobuf> _receive_proto       = nullptr;  ///< Reception protobuf.
    uint8_t _ack_size_list                                      = 32;
    uint16_t _transmit_area                                     = ProtobufIndex::INVALID;
    uint16_t _receive_area                                      = ProtobufIndex::INVALID;

   private:
    enum class State {
        IDLE,
        REACTIVE,
        ACTIVE,
        WAITING_ACK
    };
    State Idle(void);
    State Reactive(void);
    State Active(void);
    State communication_state = State::IDLE;

   private:
    uint16_t _received_bytes = 0;       ///<
    uint8_t _ack_retries     = 0;       ///<
    uint16_t _address        = 0xFFFF;  ///< Memory Address of this device
};

#endif /* COMMUNICATION_PROCESS_H */

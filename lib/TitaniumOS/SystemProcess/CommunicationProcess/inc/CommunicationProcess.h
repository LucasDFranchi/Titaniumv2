#ifndef COMMUNICATION_PROCESS_H
#define COMMUNICATION_PROCESS_H

#include "Application/error/error_enum.h"
#include "Drivers/DriverInterface/ICommunicationDriver.h"
#include "HAL/memory/SharedMemoryManager.h"
#include "Protocols/Protobuf/inc/titanium.pb.h"
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
                              memory_areas_t single_packet,
                              memory_areas_t continuos_packet);
    void Configure(uint16_t address);

   public:
    uint32_t ack_list[32] = {0};

   private:
    titan_err_t Initialize(void);
    void Execute(void);

    void ProcessState(void);
    bool IsReadyToRead(void);
    bool IsReadyToTransmitSingle(void);

    bool CheckAddressPackage(uint16_t address);

    titan_err_t ProcessReceivedPackage(std::unique_ptr<TitaniumPackage>& package);

   private:
    uint8_t* _buffer_in                                         = nullptr;  ///< Buffer for communication RX.
    uint8_t* _buffer_out                                        = nullptr;  ///< Buffer for communication TX.
    TaskHandle_t _process_handler                               = nullptr;  ///< Handler for the process task.
    IDriverInterface* _driver                                   = nullptr;  ///< Communication driver.
    std::unique_ptr<SharedMemoryManager> _shared_memory_manager = nullptr;  ///< Shared memory manager.
    TitaniumProtocol* _protocol                                 = nullptr;  ///< Protocol handler.
    uint8_t _ack_size_list                                      = 32;
    memory_areas_t _single_packet;
    memory_areas_t _continuos_packet;
    continuos_packet_list_t _cp_list{};

   private:
    enum class State {
        IDLE,
        READ,
        SINGLE,
        CONTINUOS,
    };
    State Idle(void);
    State Read(void);
    State Single(void);
    State Continuos(void);
    State communication_state = State::IDLE;

   private:
    uint16_t _received_bytes = 0;       ///<
    uint8_t _ack_retries     = 0;       ///<
    uint16_t _address        = 0xFFFF;  ///< Memory Address of this device
};

#endif /* COMMUNICATION_PROCESS_H */

#ifndef COMMUNICATION_PROCESS_H
#define COMMUNICATION_PROCESS_H

#include "Drivers/DriverInterface/ICommunicationDriver.h"
#include "HAL/memory/SharedMemoryManager.h"
#include "SystemProcess/ProcessAreasIndex.h"
#include "SystemProcess/Template/ProcessTemplate.h"
#include "Protocols/Titanium/TitaniumPackage.h"

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
        : ProcessTemplate(name, memory, priority, &this->_process_handler){};

    /**
     * @brief Installs the communication driver and configures memory areas for read and write.
     *
     * @param[in] driver_interface Pointer to the communication driver interface.
     * @param[in] memory_area_read Index of the memory area for read operations.
     * @param[in] memory_area_write Index of the memory area for write operations.
     */
    void InstallDriver(IDriverInterface* driver_interface, uint8_t memory_area_read, uint8_t memory_area_write);

   private:
    /**
     * @brief Executes the communication process.
     */
    void Execute(void);

    /**
     * @brief Initializes the communication manager.
     *
     * @return ESP_OK if initialization is successful, otherwise an error code.
     */
    esp_err_t Initialize(void);

    /**
     * @brief Stores a received package into shared memory.
     *
     * @param[in] package The package to store.
     * @return ESP_OK if storing is successful, otherwise an error code.
     */
    esp_err_t StorePackage(std::unique_ptr<TitaniumPackage>& package);

    /**
     * @brief Generates a response package based on the specified memory area.
     *
     * @param[in] memory_area The memory area to generate the response from.
     * @return A unique pointer to the generated TitaniumPackage.
     */
    std::unique_ptr<TitaniumPackage> GenerateResponsePackage(uint8_t memory_area);

    /**
     * @brief Generates a transmission package.
     *
     * @return A unique pointer to the generated TitaniumPackage.
     */
    std::unique_ptr<TitaniumPackage> GenerateTransmissionPackage(void);

    /**
     * @brief Handles the result of an operation and sends an acknowledgment.
     *
     * @param result The result of the operation.
     */
    void Acknowledge(esp_err_t result);

   private:
    std::unique_ptr<uint8_t[]> _buffer                          = nullptr;  ///< Buffer for communication.
    TaskHandle_t _process_handler                               = NULL;     ///< Handler for the process task.
    std::unique_ptr<IDriverInterface> _driver                   = nullptr;  ///< Communication driver.
    std::unique_ptr<SharedMemoryManager> _shared_memory_manager = nullptr;  ///< Shared memory manager.

   private:
    uint8_t _memory_area_receive  = ProcessAreaIndex::INVALID;  ///< Memory area for receiving data.
    uint8_t _memory_area_transmit = ProcessAreaIndex::INVALID;  ///< Memory area for transmitting data.
};

#endif /* COMMUNICATION_PROCESS_H */

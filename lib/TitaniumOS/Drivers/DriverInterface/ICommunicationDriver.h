#ifndef COMMUNICATION_DRIVER_INTERFACE_H
#define COMMUNICATION_DRIVER_INTERFACE_H

#include "esp_err.h"

/**
 * @brief Interface for communication driver.
 *
 * This interface defines the basic methods for a communication driver.
 */
class IDriverInterface {
   public:
    /**
     * @brief Virtual destructor for the interface.
     */
    virtual ~IDriverInterface() {}

    /**
     * @brief Write data to the communication interface.
     *
     * @param raw_bytes Pointer to the raw bytes to be written.
     * @param size Number of bytes to write.
     * @return esp_err_t ESP_OK on success, or an error code on failure.
     */
    virtual esp_err_t Write(uint8_t* raw_bytes, uint16_t size) = 0;

    /**
     * @brief Read data from the communication interface.
     *
     * @param raw_bytes Pointer to the buffer where the read bytes will be stored.
     * @return uint16_t Number of bytes read.
     */
    virtual uint16_t Read(uint8_t* raw_bytes) = 0;

    /**
     * @brief Get the size of the buffer.
     *
     * @return uint16_t Size of the buffer.
     */
    virtual uint16_t buffer_size() const = 0;
};

#endif  // COMMUNICATION_DRIVER_INTERFACE_H

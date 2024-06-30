#ifndef TITANIUM_PACKAGE_H
#define TITANIUM_PACKAGE_H

#include <memory>

#include "esp_err.h"

#include "TitaniumProtocolEnums.h"

#include "HAL/memory/MemoryHandlers.h"

/**
 * @brief Represents a Titanium package containing data and metadata.
 *
 * This class encapsulates a Titanium package, which consists of data along with metadata
 * such as package size, command type, and memory area. The package data is stored in a
 * dynamically allocated buffer managed by a std::unique_ptr.
 */
class TitaniumPackage {
   public:
    /**
     * @brief Constructs a TitaniumPackage object.
     *
     * Constructs a TitaniumPackage object with the specified size, command type, memory area,
     * and data. The data provided is copied into an internal buffer.
     *
     * @param[in] size The size of the package data.
     * @param[in] command The command type associated with the package.
     * @param[in] memory_area The memory area identifier associated with the package.
     * @param[in] data A pointer to the data to be copied into the package.
     */
    TitaniumPackage(uint16_t size, command_e command, uint8_t memory_area, uint8_t* data)
        : _command(command), _memory_area(memory_area) {
        uint16_t real_size = this->TrimTrailingZeros(data, size);
        this->_size        = real_size;
        this->_data        = std::make_unique<uint8_t[]>(real_size);
        memcpy_s<uint8_t>(this->_data.get(), data, real_size);
    }
    /**
     * @brief Retrieves the package data.
     *
     * Copies the package data into the provided buffer.
     *
     * @param[in] data A pointer to the buffer where the package data will be copied.
     * @return The size of the package data copied, or 0 if an error occurs.
     */
    uint16_t Consume(uint8_t* data) {
        auto result = memcpy_s<uint8_t>(data, this->_data.get(), this->_size);

        if (result == ESP_OK) {
            return this->_size;
        }

        return 0;
    }

    /**
     * @brief Get the size of the package data.
     *
     * @return The size of the package data.
     */
    uint16_t size() const {
        return _size;
    }

    /**
     * @brief Get the command type associated with the package.
     *
     * @return The command type associated with the package.
     */
    command_e command() const {
        return _command;
    }

    /**
     * @brief Get the memory area identifier associated with the package.
     *
     * @return The memory area identifier associated with the package.
     */
    uint8_t memory_area() const {
        return _memory_area;
    }

   private:
    /**
     * @brief Trims trailing zeros from a buffer and returns the size of the data without trailing zeros.
     *
     * @param[in] buffer The buffer containing the data.
     * @param[in] maximum_size The maximum size of the buffer.
     * @return uint16_t The size of the data without trailing zeros.
     *
     * This function iterates through the buffer from the end towards the beginning to find the
     * last non-zero byte. It then returns the length of the buffer excluding any trailing zeros.
     * If the buffer is NULL, it returns 0.
     */
    uint16_t TrimTrailingZeros(uint8_t* buffer, uint16_t maximum_size) {
        if (buffer == nullptr) {
            return 0;
        }

        uint16_t real_size = maximum_size;

        // while (real_size > 0 && buffer[real_size - 1] == 0) {
        //     real_size--;
        // }

        return real_size;
    }

   private:
    uint16_t _size       = 0;                  ///< The size of the package data.
    command_e _command   = INVALID_OPERATION;  ///< The command type associated with the package.
    uint8_t _memory_area = -1;                 ///< The memory area identifier associated with the package.
    std::unique_ptr<uint8_t[]> _data;          ///< The buffer storing the package data.
};

#endif /* TITANIUM_PACKAGE_H */
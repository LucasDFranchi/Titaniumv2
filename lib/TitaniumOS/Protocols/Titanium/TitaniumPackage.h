#ifndef TITANIUM_PACKAGE_GUARD
#define TITANIUM_PACKAGE_GUARD

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
        : _size(size), _command(command), _memory_area(memory_area), _data(std::make_unique<uint8_t[]>(size)) {
        memcpy_s<uint8_t>(this->_data.get(), data, size);
    };

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
    uint16_t _size       = 0;                  ///< The size of the package data.
    command_e _command   = INVALID_OPERATION;  ///< The command type associated with the package.
    uint8_t _memory_area = -1;                 ///< The memory area identifier associated with the package.
    std::unique_ptr<uint8_t[]> _data;          ///< The buffer storing the package data.
};

#endif /* TITANIUM_PROTOCOL_GUARD */
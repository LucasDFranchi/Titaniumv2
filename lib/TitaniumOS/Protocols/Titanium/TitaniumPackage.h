#ifndef TITANIUM_PACKAGE_H
#define TITANIUM_PACKAGE_H

#include "esp_err.h"
#include "esp_log.h"
#include "esp_random.h"
#include <memory>

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
     * @param[in] address The address associated with the package.
     * @param[in] memory_area The memory area identifier associated with the package.
     * @param[in] data A pointer to the data to be copied into the package.
     */
    TitaniumPackage(uint16_t size, uint16_t address, uint8_t memory_area, uint8_t* data)
        : _memory_area(memory_area) {
        this->_size    = size;
        this->_address = address;
        this->_data    = std::make_unique<uint8_t[]>(size);
        this->_uuid    = this->GenerateUUID();
        memcpy_s(this->_data.get(), data, size);
    }
    /**
     * @brief Constructs a TitaniumPackage object.
     *
     * Constructs a TitaniumPackage object with the specified size, command type, memory area,
     * and data. The data provided is copied into an internal buffer.
     *
     * @param[in] size The size of the package data.
     * @param[in] address The address associated with the package.
     * @param[in] memory_area The memory area identifier associated with the package.
     * @param[in] data A pointer to the data to be copied into the package.
     */
    TitaniumPackage(uint16_t size, uint16_t address, uint8_t memory_area, char* data)
        : _memory_area(memory_area) {
        this->_size    = size;
        this->_address = address;
        this->_data    = std::make_unique<uint8_t[]>(size);
        this->_uuid    = this->GenerateUUID();
        memcpy_s(this->_data.get(), data, size);
    }
    /**
     * @brief Retrieves the package data.
     *
     * Copies the package data into the provided buffer.
     *
     * @param[in] data_out A pointer to the buffer where the package data will be copied.
     * @return The size of the package data copied, or 0 if an error occurs.
     */
    uint16_t Consume(uint8_t* data_out) {
        uint16_t result = 0;
        do {
            if (this->_data.get() == nullptr) {
                break;
            }

            if (data_out == nullptr) {
                break;
            }

            if (memcpy_s(data_out, this->_data.get(), this->_size) != ESP_OK) {
                break;
            }
            result = this->_size;
        } while (0);

        return result;
    }

    uint32_t GenerateUUID() {
        return esp_random();
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
     * @brief Get the address of the package data.
     *
     * @return The address of the package data.
     */
    uint16_t address() const {
        return _address;
    }

    /**
     * @brief Get the memory area identifier associated with the package.
     *
     * @return The memory area identifier associated with the package.
     */
    uint8_t memory_area() const {
        return _memory_area;
    }

    /**
     * @brief Get the UUID the package, to make the protocol robust and
     *        compact we re-use the crc32 as UUID.
     *
     * @return The UUID associated with the package.
     */
    uint32_t uuid() const {
        return _uuid;
    }

   private:
    uint16_t _size       = 0;                  ///< The size of the package data.
    uint32_t _uuid       = -1;                 ///< The UUID associated with the package.
    uint16_t _address    = 0;                  ///< The address of the transmitted package.
    uint8_t _memory_area = -1;                 ///< The memory area identifier associated with the package.
    std::unique_ptr<uint8_t[]> _data;          ///< The buffer storing the package data.
};

#endif /* TITANIUM_PACKAGE_H */
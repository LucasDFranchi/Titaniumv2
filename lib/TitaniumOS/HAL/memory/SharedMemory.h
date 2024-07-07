#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include "MemoryHandlers.h"
#include "MemoryTypes.h"
#include "esp_err.h"

#include <memory>

/**
 * @brief Template for a memory area.
 */
class SharedMemory {
   public:
    /**
     * @brief Constructs a new SharedMemory object.
     *
     * @param index Index of the memory area.
     * @param size Size of the memory area.
     * @param access_type Access type of the memory area.
     */
    SharedMemory(uint8_t index, uint16_t size, AccessType access_type) {
        this->_index         = index;
        this->_size          = size;
        this->_access_type   = access_type;
        this->_has_update    = false;
        this->_mutex         = xSemaphoreCreateMutex();
        this->_written_bytes = 0;
        this->_data.reset(new uint8_t[size]);

        this->Clear();
    }
    /**
     * @brief Clears the memory area by setting all its data to zero.
     *
     * @return esp_err_t Error code indicating the result of the operation.
     */
    esp_err_t Clear(void) {
        return memset_s(this->_data.get(), 0, this->_size);
    }
    /**
     * @brief Retrieves the size of the memory area.
     *
     * @return uint16_t The size of the memory area.
     */
    uint16_t GetSize(void) {
        return this->_size;
    }

    /**
     * @brief Retrieves the amount of bytes written in that memory area.
     *
     * @return uint16_t The amount of written bytes.
     */
    uint16_t GetWrittenBytes(void) {
        return this->_written_bytes;
    }

    /**
     * @brief Retrieves the access type of the memory area.
     *
     * @return AccessType The access type of the memory area.
     */
    AccessType GetAccess(void) {
        return this->_access_type;
    }

    /**
     * @brief Retrieves the index of the memory area.
     *
     * @return uint8_t The index of the memory area.
     */
    uint8_t GetAreaIndex(void) {
        return this->_index;
    }

    /**
     * @brief Retrieves if an area was read.
     *
     * @return bool True if the area has been read already, false otherwise.
     */
    bool GetAreaHasUpdated(void) {
        return this->_has_update;
    }

    template <typename T>
    esp_err_t Write(T* protobuf) {
        auto serialized_size = 0;

        if (this->_access_type == READ_ONLY) {
            return ESP_FAIL;
        }

        if (this->_mutex != NULL) {
            if (xSemaphoreTake(this->_mutex, portMAX_DELAY) == pdTRUE) {
                memset_s<uint8_t>(this->_data.get(), 0, this->_size);
                serialized_size      = protobuf->Serialize(reinterpret_cast<char*>(this->_data.get()), this->_size);
                this->_has_update    = true;
                this->_written_bytes = serialized_size;

                xSemaphoreGive(this->_mutex);
            }
        }
        return serialized_size != 0 ? ESP_OK : ESP_FAIL;
    }

    /**
     * @brief Writes data to the memory area.
     *
     * @param[in] pointer_data Pointer to the data to write.
     * @param[in] size Size of the data to write.
     * @return esp_err_t Error code indicating the result of the operation.
     */
    esp_err_t Write(uint8_t* pointer_data, size_t size) {
        auto result = ESP_FAIL;

        if (this->_access_type == READ_ONLY) {
            return result;
        }

        if (this->_mutex != NULL) {
            if (xSemaphoreTake(this->_mutex, portMAX_DELAY) == pdTRUE) {
                memset_s<uint8_t>(this->_data.get(), 0, size);
                result               = memcpy_s<uint8_t>(this->_data.get(), pointer_data, size);
                this->_has_update    = true;
                this->_written_bytes = size;

                xSemaphoreGive(this->_mutex);
            }
        }
        return result;
    }

    template <typename T>
    esp_err_t Read(T* protobuf) {
        auto result = ESP_FAIL;

        if (this->_access_type == WRITE_ONLY) {
            return result;
        }

        if (this->_mutex != NULL) {
            if (xSemaphoreTake(this->_mutex, portMAX_DELAY) == pdTRUE) {
                result            = protobuf->DeSerialize(reinterpret_cast<char*>(this->_data.get()), this->_written_bytes);
                this->_has_update = false;
                xSemaphoreGive(this->_mutex);
            }
        }

        return result;
    }

    /**
     * @brief Reads data from the memory area.
     *
     * @param[out] pointer_data Pointer to the buffer where the read data will
     * be stored.
     * @return esp_err_t Error code indicating the result of the operation.
     */
    esp_err_t Read(uint8_t* pointer_data) {
        auto result = ESP_FAIL;

        if (this->_access_type == WRITE_ONLY) {
            return result;
        }

        if (this->_mutex != NULL) {
            if (xSemaphoreTake(this->_mutex, portMAX_DELAY) == pdTRUE) {
                result            = memcpy_s<uint8_t>(pointer_data, this->_data.get(), this->_written_bytes);
                this->_has_update = false;
                xSemaphoreGive(this->_mutex);
            }
        }

        return result;
    }

   protected:
    uint8_t _index;                     /**< Index of the memory area. */
    AccessType _access_type;            /**< Access type of the memory area. */
    std::unique_ptr<uint8_t[]> _data;   /**< Pointer to the data buffer. */
    uint8_t _has_update;                /**< Flag indicating if the area has been updated. */
    uint8_t _written_bytes;             /**< Number of valid bytes written in that area. */
    uint16_t _size;                     /**< Size of the memory area. */
    SemaphoreHandle_t _mutex = nullptr; /**< Mutex semaphore for thread safety. */
};

#endif /* SHARED_MEMORY_H */

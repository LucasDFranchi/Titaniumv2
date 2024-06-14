
#ifndef SHARED_MEMORY_GUARD
#define SHARED_MEMORY_GUARD

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include "MemoryHandlers.h"
#include "MemoryTypes.h"
#include "esp_err.h"

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
        this->_index = index;
        this->_data = new uint8_t[size];
        this->_size = size;
        this->_access_type = access_type;
        this->_has_update = false;
        this->_mutex = xSemaphoreCreateMutex();

        this->Clear();
    }
    /**
     * @brief Clears the memory area by setting all its data to zero.
     *
     * @return esp_err_t Error code indicating the result of the operation.
     */
    esp_err_t Clear(void) { return memset_s(this->_data, 0, this->_size); }
    /**
     * @brief Retrieves the size of the memory area.
     *
     * @return uint16_t The size of the memory area.
     */
    uint16_t GetSize(void) { return this->_size; }

    /**
     * @brief Retrieves the access type of the memory area.
     *
     * @return AccessType The access type of the memory area.
     */
    AccessType GetAccess(void) { return this->_access_type; }

    /**
     * @brief Retrieves the index of the memory area.
     *
     * @return uint8_t The index of the memory area.
     */
    uint8_t GetAreaIndex(void) { return this->_index; }

    /**
     * @brief Retrieves if an area was read.
     *
     * @return bool True if the area has been read already, false otherwise.
     */
    bool GetAreaHasUpdated(void) { return this->_has_update; }

    /**
     * @brief Writes data to the memory area.
     *
     * @param[in] data_pointer Pointer to the data to write.
     * @param[in] size Size of the data to write.
     * @return esp_err_t Error code indicating the result of the operation.
     */
    esp_err_t Write(uint8_t* data_pointer, size_t size) {
        auto result = ESP_FAIL;

        if (this->_access_type == READ_ONLY) {
            return result;
        }

        if (this->_mutex != NULL) {
            if (xSemaphoreTake(this->_mutex, portMAX_DELAY) == pdTRUE) {
                result = memcpy_s<uint8_t>(this->_data, data_pointer, size);
                this->_has_update = true;

                xSemaphoreGive(this->_mutex);
            }
        }
        return result;
    }

    /**
     * @brief Reads data from the memory area.
     *
     * @param[out] data_pointer Pointer to the buffer where the read data will
     * be stored.
     * @return esp_err_t Error code indicating the result of the operation.
     */
    esp_err_t Read(uint8_t* data_pointer) {
        auto result = ESP_FAIL;

        if (this->_access_type == WRITE_ONLY) {
            return result;
        }

        if (this->_mutex != NULL) {
            if (xSemaphoreTake(this->_mutex, portMAX_DELAY) == pdTRUE) {
                result = memcpy_s<uint8_t>(data_pointer, this->_data, this->_size);
                this->_has_update = false;
                xSemaphoreGive(this->_mutex);
            }
        }

        return result;
    }

   protected:
    uint8_t _index;
    AccessType _access_type;
    uint8_t* _data;
    uint8_t _has_update;
    uint16_t _size;
    SemaphoreHandle_t _mutex = nullptr;
};

#endif /* MEMORY_AREA_GUARD */
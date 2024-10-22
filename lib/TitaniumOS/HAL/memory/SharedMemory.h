#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <freertos/FreeRTOS.h>
#include "freertos/semphr.h"
#include <freertos/task.h>

#include "Application/error/error_enum.h"
#include "MemoryHandlers.h"
#include "MemoryTypes.h"

#include "pb_decode.h"
#include "pb_encode.h"

#include "esp_log.h"

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
     * @param should_transmit Indicates if this area should be transmitted trough IoT.
     */
    SharedMemory(uint8_t index, uint16_t size, AccessType access_type, bool should_transmit) {
        this->_index           = index;
        this->_size            = size;
        this->_access_type     = access_type;
        this->_should_transmit = should_transmit;
        this->_has_update      = false;
        this->_mutex           = xSemaphoreCreateMutex();
        this->_written_bytes   = 0;
        this->_data            = new uint8_t[size];

        this->Clear();
    }
    /**
     * @brief Clears the memory area by setting all its data to zero.
     *
     * @return titan_err_t Error code indicating the result of the operation.
     */
    titan_err_t Clear(void) {
        return memset_s(this->_data, 0, this->_size);
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
     * @brief Retrieves the flag that indicates if this area should be transmitted over IoT.
     *
     * @return true if the area should be transmitted over IoT, otherwise false.
     */
    bool GetShouldTransmit(void) {
        return this->_should_transmit;
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
    titan_err_t Write(T& protobuf, const pb_msgdesc_t& msg_desc) {
        if (this->_access_type == READ_ONLY) {
            return Error::UNKNOW_FAIL;
        }

        if (this->_mutex != NULL) {
            if (xSemaphoreTake(this->_mutex, portMAX_DELAY) == pdTRUE) {
                memset_s(this->_data, 0, this->_size);
                pb_ostream_t ostream = pb_ostream_from_buffer(this->_data, this->_size);
                auto ret             = pb_encode(&ostream, ((pb_msgdesc_t*)&msg_desc), &protobuf);

                if (ret) {
                    this->_has_update    = true;
                    this->_written_bytes = ostream.bytes_written;
                } else {
                    ESP_LOGE("Shared Memory", "Error encoding message %d: %s!", this->_index, ostream.errmsg);
                    this->_written_bytes = 0;
                }

                xSemaphoreGive(this->_mutex);
            }
        }
        return this->_written_bytes > 0 ? Error::NO_ERROR : Error::WRITTEN_LESS_THAN_ZERO;
    }

    titan_err_t Write(char* buffer, uint16_t written_bytes) {
        if ((this->_access_type == READ_ONLY) || (buffer == nullptr)) {
            return Error::UNKNOW_FAIL;
        }

        if (this->_mutex != NULL) {
            if (xSemaphoreTake(this->_mutex, portMAX_DELAY) == pdTRUE) {
                memset_s(this->_data, 0, this->_size);
                memcpy(this->_data, buffer, written_bytes);

                this->_has_update    = true;
                this->_written_bytes = written_bytes;

                xSemaphoreGive(this->_mutex);
            }
        }
        return this->_written_bytes > 0 ? Error::NO_ERROR : Error::UNKNOW_FAIL;
    }

    template <typename T>
    titan_err_t Read(T& protobuf, const pb_msgdesc_t& msg_desc, bool silent) {
        auto result = Error::UNKNOW_FAIL;

        if (this->_access_type == WRITE_ONLY) {
            return result;
        }

        if (this->_written_bytes == 0) {
            return Error::EMPTY_PROTOBUF;
        }

        if (this->_mutex != NULL) {
            if (xSemaphoreTake(this->_mutex, portMAX_DELAY) == pdTRUE) {
                pb_istream_t istream = pb_istream_from_buffer(this->_data, this->_written_bytes);
                auto ret             = pb_decode(&istream, ((pb_msgdesc_t*)&msg_desc), &protobuf);

                if (ret) {
                    this->_has_update = !silent ? false : this->_has_update;
                    result            = Error::NO_ERROR;
                } else {
                    ESP_LOGE("Shared Memory", "Failed to decode message %d: %s!", this->_index, istream.errmsg);
                    result = Error::DESERIALIZE_ERROR;
                }

                xSemaphoreGive(this->_mutex);
            }
        }

        return result;
    }

    titan_err_t Read(char* buffer, bool silent) {
        auto result = Error::UNKNOW_FAIL;

        if ((this->_access_type == WRITE_ONLY) || (buffer == nullptr)) {
            return result;
        }

        if (this->_written_bytes == 0) {
            return Error::EMPTY_PROTOBUF;
        }

        if (this->_mutex != nullptr) {
            if (xSemaphoreTake(this->_mutex, portMAX_DELAY) == pdTRUE) {
                result = memcpy_s(buffer, this->_data, this->_written_bytes);

                if (!silent) {
                    this->_has_update = false;
                }
                xSemaphoreGive(this->_mutex);
            }
        }

        return result;
    }

   protected:
    uint8_t _index;                     /**< Index of the memory area. */
    AccessType _access_type;            /**< Access type of the memory area. */
    bool _should_transmit;              /**< Flag that indicates if this area should be transmitted. */
    uint8_t* _data;                     /**< Pointer to the data buffer. */
    uint8_t _has_update;                /**< Flag indicating if the area has been updated. */
    uint8_t _written_bytes;             /**< Number of valid bytes written in that area. */
    uint16_t _size;                     /**< Size of the memory area. */
    SemaphoreHandle_t _mutex = nullptr; /**< Mutex semaphore for thread safety. */
};

#endif /* SHARED_MEMORY_H */

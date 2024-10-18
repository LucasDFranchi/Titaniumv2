#ifndef SHARED_MEMORY_MANAGER_H
#define SHARED_MEMORY_MANAGER_H

#include <stdint.h>
#include <memory>

#include "Application/error/error_enum.h"
#include "SharedMemory.h"

/**
 * @brief Manages shared memory used for services to exchange data.
 */
class SharedMemoryManager {
   public:
    SharedMemoryManager(const SharedMemoryManager& obj) = delete;
    static SharedMemoryManager* GetInstance(void);

   public:
    titan_err_t Initialize(void);
    bool IsAreaDataUpdated(uint8_t area_index);
    bool ShouldTransmitArea(uint8_t area_index);
    uint16_t GetAreaSize(uint8_t area_index);
    uint16_t GetWrittenBytes(uint8_t area_index);
    uint16_t GetNumAreas(void);
    titan_err_t SignUpSharedArea(uint8_t index,
                                 uint16_t size_in_bytes,
                                 AccessType access_type,
                                 bool should_transmit = true);

   private:
    SharedMemoryManager() {};
    static SharedMemoryManager* singleton_pointer_;

   private:
    static constexpr uint16_t _maximum_shared_memory = 32;
    uint16_t _num_areas                              = 0;
    std::unique_ptr<SharedMemory> _shared_memory_array[SharedMemoryManager::_maximum_shared_memory];

   public:
    /**
     * Writes data to a specific memory area.
     *
     * @param[in] area_index The index of the memory area to write to.
     * @param[in] size The size of the data to write.
     * @param[in] protobuf A pointer to the data to write.
     *
     * @returns An titan_err_t indicating the result of the write operation.
     *          - ESP_OK if the write operation was successful.
     *          - Error::UNKNOW_FAIL if an error occurred during the write operation.
     */
    template <typename T>
    titan_err_t Write(uint8_t area_index, T& protobuf, const pb_msgdesc_t& msg_desc) {
        titan_err_t result = Error::UNKNOW_FAIL;

        do {
            if (area_index > this->_maximum_shared_memory) {
                result = Error::INVALID_MEMORY_AREA;
                break;
            }

            if (this->_shared_memory_array[area_index] == nullptr) {
                result = Error::NULL_PTR;
                break;
            }

            result = this->_shared_memory_array[area_index]->Write(protobuf, msg_desc);

        } while (0);

        return result;
    }

    titan_err_t Write(uint8_t area_index, char* buffer, uint16_t written_bytes) {
        titan_err_t result = Error::UNKNOW_FAIL;

        do {
            if (area_index > this->_maximum_shared_memory) {
                result = Error::INVALID_MEMORY_AREA;
                break;
            }

            if (this->_shared_memory_array[area_index] == nullptr) {
                result = Error::NULL_PTR;
                break;
            }

            auto memory_size =
                this->_shared_memory_array[area_index]->GetSize();

            if (memory_size < written_bytes) {
                result = Error::BUFFER_OUT_OF_SPACE;
                break;
            }

            result = this->_shared_memory_array[area_index]->Write(buffer, written_bytes);

        } while (0);

        return result;
    }

    template <typename T>
    uint16_t Read(uint8_t area_index, T& protobuf, const pb_msgdesc_t& msg_desc, bool silent = false) {
        uint16_t result = 0;

        do {
            if (area_index > this->_maximum_shared_memory) {
                break;
            }

            if (this->_shared_memory_array[area_index] == nullptr) {
                break;
            }

            if (this->_shared_memory_array[area_index]->Read(protobuf, msg_desc, silent) != Error::NO_ERROR) {
                break;
            }

            result = this->_shared_memory_array[area_index]->GetWrittenBytes();

        } while (0);

        return result;
    }

    uint16_t Read(uint8_t area_index, char* buffer, uint16_t buffer_size, bool silent = false) {
        uint16_t result = 0;

        do {
            if (area_index > this->_maximum_shared_memory) {
                break;
            }

            if (this->_shared_memory_array[area_index] == nullptr) {
                break;
            }

            auto memory_size =
                this->_shared_memory_array[area_index]->GetSize();
            if (memory_size > buffer_size) {
                break;
            }

            if (this->_shared_memory_array[area_index]->Read(buffer, silent) != Error::NO_ERROR) {
                break;
            }

            result = this->_shared_memory_array[area_index]->GetWrittenBytes();

        } while (0);

        return result;
    }
};

#endif /* SHARED_MEMORY_MANAGER_H */
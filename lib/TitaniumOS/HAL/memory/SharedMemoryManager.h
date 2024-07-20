#ifndef SHARED_MEMORY_MANAGER_H
#define SHARED_MEMORY_MANAGER_H

#include <stdint.h>
#include <memory>

#include "Kernel/error/error_enum.h"
#include "Protocols/Protobuf/inc/IProtobuf.h"
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
    titan_err_t SignUpSharedArea(uint8_t index, uint16_t size_in_bytes, AccessType access_type);
    bool IsAreaDataUpdated(uint8_t area_index);
    uint16_t GetAreaSize(uint8_t area_index);
    uint16_t GetWrittenBytes(uint8_t area_index);
    uint16_t GetNumAreas(void);

   private:
    SharedMemoryManager() {};
    static SharedMemoryManager* singleton_pointer_;

   private:
    static constexpr uint16_t _maximum_shared_memory = 32;
    uint16_t _num_areas                              = 0;
    // std::unique_ptr<std::unique_ptr<SharedMemory>[]> _shared_memory_array;
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
    titan_err_t Write(uint8_t area_index, IProtobuf& protobuf) {
        titan_err_t result = Error::UNKNOW_FAIL;

        do {
            if (area_index > this->_maximum_shared_memory) {
                result = ESP_ERR_INVALID_ARG;
                break;
            }

            if (this->_shared_memory_array[area_index] == nullptr) {
                result = ESP_ERR_NO_MEM;
                break;
            }

            auto memory_size =
                this->_shared_memory_array[area_index]->GetSize();

            if (memory_size < protobuf.GetSerializedSize()) {
                result = ESP_ERR_INVALID_SIZE;
                break;
            }

            result = this->_shared_memory_array[area_index]->Write(protobuf);

        } while (0);

        return result;
    }

    uint16_t Read(uint8_t area_index, IProtobuf& protobuf) {
        uint16_t result = 0;

        do {
            if (area_index > this->_maximum_shared_memory) {
                result = ESP_ERR_INVALID_ARG;
                break;
            }

            if (this->_shared_memory_array[area_index] == nullptr) {
                result = ESP_ERR_NO_MEM;
                break;
            }

            auto memory_size =
                this->_shared_memory_array[area_index]->GetSize();
            if (memory_size > protobuf.GetMaxSize()) {
                result = ESP_ERR_INVALID_SIZE;
                break;
            }

            if (this->_shared_memory_array[area_index]->Read(protobuf) != ESP_OK) {
                break;
            }

            result = ESP_OK;

        } while (0);

        return result;
    }
};

#endif /* SHARED_MEMORY_MANAGER_H */
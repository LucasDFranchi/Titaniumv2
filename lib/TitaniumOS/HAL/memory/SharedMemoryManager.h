#ifndef SHARED_MEMORY_MANAGER_GUARD
#define SHARED_MEMORY_MANAGER_GUARD

#include <stdint.h>

#include <memory>

#include "SharedMemory.h"
#include "esp_err.h"

/**
 * @brief Manages shared memory used for services to exchange data.
 */
class SharedMemoryManager {
    public:
    SharedMemoryManager(const SharedMemoryManager& obj) = delete;
    static SharedMemoryManager* GetInstance(void);

    public:
    esp_err_t Initialize(void);
    esp_err_t SignUpSharedArea(uint8_t index, uint16_t size_in_bytes, AccessType access_type);
    bool      IsAreaDataUpdated(uint8_t area_index);

    private:
    SharedMemoryManager(){};
    static SharedMemoryManager* singleton_pointer_;

   private:
    static constexpr uint16_t _maximum_shared_memory = 32;
    // std::unique_ptr<std::unique_ptr<SharedMemory>[]> _shared_memory_array;
    std::unique_ptr<SharedMemory> _shared_memory_array[SharedMemoryManager::_maximum_shared_memory];


   public:
    /**
     * Writes data to a specific memory area.
     *
     * @param[in] area_index The index of the memory area to write to.
     * @param[in] size The size of the data to write.
     * @param[in] pointer_data A pointer to the data to write.
     *
     * @returns An esp_err_t indicating the result of the write operation.
     *          - ESP_OK if the write operation was successful.
     *          - ESP_FAIL if an error occurred during the write operation.
     */
    template <typename T>
    esp_err_t Write(uint8_t area_index, uint16_t size, T* pointer_data) {
        esp_err_t result = ESP_FAIL;

        do {
            if (pointer_data == nullptr) {
                result = ESP_ERR_NO_MEM;
                break;
            }

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
            if (memory_size < size) {
                result = ESP_ERR_INVALID_SIZE;
                break;
            }

            result = this->_shared_memory_array[area_index]->Write(
                reinterpret_cast<uint8_t*>(pointer_data), size);

        } while (0);

        return result;
    }
    /**
     * Reads data from a specific memory area.
     *
     * @param[in] area_index The index of the memory area to read from.
     * @param[in] pointer_data Pointer to the buffer where the read data will be
     * stored.
     *
     * @returns ESP_OK if the read operation is successful, otherwise an error
     * code.
     */
    template <typename T>
    uint16_t Read(uint8_t area_index, T* pointer_data) {
        uint16_t result = 0;

        do {
            if (area_index > this->_maximum_shared_memory) {
                break;
            }
            if (this->_shared_memory_array[area_index]->Read(
                    reinterpret_cast<uint8_t*>(pointer_data)) != ESP_OK) {
                break;
            }

            result = this->_shared_memory_array[area_index]->GetSize();

        } while (0);

        return result;
    }
};

#endif /* SHARED_MEMORY_MANAGER_GUARD */
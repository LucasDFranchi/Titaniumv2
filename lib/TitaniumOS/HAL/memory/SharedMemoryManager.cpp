#include "SharedMemoryManager.h"

/**
 * @brief Initializes the MemoryManager.
 *
 * @return esp_err_t ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t SharedMemoryManager::Initialize(void) {
    for (uint16_t i = 0; i < this->_maximum_shared_memory; i++) {
        this->_shared_memory_array[i] = nullptr;
    }

    return ESP_OK;
}

/**
 * @brief Registers a shared memory area.
 *
 * @param index Index of the shared memory area.
 * @param size_in_bytes Size of the shared memory area in bytes.
 * @param access_type Access type of the shared memory area.
 * @return esp_err_t Error code indicating the result of the operation.
 */
esp_err_t SharedMemoryManager::SignUpSharedArea(uint8_t index, uint16_t size_in_bytes,
                                                AccessType access_type) {
    esp_err_t result = ESP_FAIL;

    do {
        if (index >= this->_maximum_shared_memory) {
            result = ESP_ERR_INVALID_ARG;
            break;
        }

        if (this->_shared_memory_array[index] != nullptr) {
            result = ESP_ERR_INVALID_STATE;
            break;
        }

        this->_shared_memory_array[index] = std::make_unique<SharedMemory>(index, size_in_bytes, access_type);
        result                            = ESP_OK;

    } while (0);

    return result;
}

/**
 * @brief Checks if the data in a memory area was updated.
 *
 * @param[in] area_index Index of the memory area to check.
 * @return bool True if the data was updated, false otherwise.
 */
bool SharedMemoryManager::IsAreaDataUpdated(uint8_t area_index) {
    return this->_shared_memory_array[area_index]->GetAreaHasUpdated();
}

/**
 * @brief Retrieves the size of the specified memory area.
 *
 * This function returns the size of the memory area at the given index within
 * the shared memory array.
 *
 * @param[in] area_index The index of the memory area whose size is to be retrieved.
 *                   The index must be within the valid range of the shared memory array.
 * @return uint16_t The size of the memory area at the specified index.
 */
uint16_t SharedMemoryManager::GetAreaSize(uint8_t area_index) {
    return this->_shared_memory_array[area_index]->GetSize();
}

/**
 * @brief Retrieves the size of the specified memory area.
 *
 * This function returns the amount of bytes written in that memory area at the given index within
 * the shared memory array.
 *
 * @param[in] area_index The index of the memory area whose size is to be retrieved.
 *                   The index must be within the valid range of the shared memory array.
 * @return uint16_t The amount of written bytes.
 */
uint16_t SharedMemoryManager::GetWrittenBytes(uint8_t area_index) {
    return this->_shared_memory_array[area_index]->GetWrittenBytes();
}


/**
 * @brief Returns the singleton instance of SharedMemoryManager.
 *
 * @return SharedMemoryManager* Pointer to the singleton instance of SharedMemoryManager.
 */
SharedMemoryManager* SharedMemoryManager::GetInstance(void) {
    if (singleton_pointer_ == nullptr) {
        singleton_pointer_ = new SharedMemoryManager();
    }

    return singleton_pointer_;
}

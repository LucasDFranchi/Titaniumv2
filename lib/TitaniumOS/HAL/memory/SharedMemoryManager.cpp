#include "SharedMemoryManager.h"

/**
 * @brief Initializes the MemoryManager.
 *
 * @return titan_err_t ESP_OK if initialization is successful, otherwise an error code.
 */
titan_err_t SharedMemoryManager::Initialize(void) {
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
 * @param should_transmit Indicates if this area should be transmitted trough IoT.
 * 
 * @return titan_err_t Error code indicating the result of the operation.
 */
titan_err_t SharedMemoryManager::SignUpSharedArea(uint8_t index,
                                                  uint16_t size_in_bytes,
                                                  AccessType access_type,
                                                  bool should_transmit) {
    titan_err_t result = Error::UNKNOW_FAIL;

    do {
        if (index >= this->_maximum_shared_memory) {
            result = Error::INVALID_MEMORY_AREA;
            break;
        }

        if (this->_shared_memory_array[index] != nullptr) {
            result = Error::NULL_PTR;
            break;
        }

        this->_shared_memory_array[index] = std::make_unique<SharedMemory>(index,
                                                                           size_in_bytes,
                                                                           access_type,
                                                                           should_transmit);
        result                            = Error::NO_ERROR;
        this->_num_areas++;

        this->UpdateMaximumAreaSize(size_in_bytes);

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
    if (this->_shared_memory_array[area_index] != nullptr)
        return this->_shared_memory_array[area_index]->GetAreaHasUpdated();
    return false;
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
 * @brief Determines whether the specified memory area should be transmitted over IoT.
 *
 * This function checks if the memory area at the given index has been marked for transmission 
 * over the IoT network. Each memory area may have its own configuration specifying whether 
 * it should be sent through IoT. The index provided must be within the valid range of 
 * the shared memory array.
 *
 * @param[in] area_index The index of the memory area to check.
 *                       The index must be a valid index within the shared memory array.
 * 
 * @return true if the memory area at the specified index should be transmitted over IoT, 
 *         false otherwise.
 */
bool SharedMemoryManager::ShouldTransmitArea(uint8_t area_index) {
    return this->_shared_memory_array[area_index]->GetShouldTransmit();
}

/**
 * @brief Retrieves the total number of memory areas managed by the shared memory system.
 *
 * This function returns the number of memory areas present in the shared memory system. 
 * These areas are used for data storage and can be individually accessed and managed.
 * The total number is fixed during initialization and defines how many memory areas are available.
 *
 * @return The total number of memory areas.
 */
uint16_t SharedMemoryManager::GetNumAreas(void) {
    return this->_num_areas;
}

/**
 * @brief Updates the maximum memory area size.
 *
 * This function updates the maximum allowed size of a memory area if the given size
 * exceeds the current maximum. It ensures that the size is only increased.
 *
 * @param[in] size_in_bytes The new size in bytes to compare with the current maximum.
 */
void SharedMemoryManager::UpdateMaximumAreaSize(uint16_t size_in_bytes) {
    if (size_in_bytes > this->maximum_area_size) {
        this->maximum_area_size = size_in_bytes;
    }
}

/**
 * @brief Retrieves the maximum memory area size.
 *
 * This function returns the maximum allowed size of a memory area.
 *
 * @return uint16_t The current maximum size of the memory area in bytes.
 */
uint16_t SharedMemoryManager::GetMaximumAreaSize(void) {
    return this->maximum_area_size;
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

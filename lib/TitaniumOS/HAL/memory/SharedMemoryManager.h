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
    void UpdateMaximumAreaSize(uint16_t size_in_bytes);
    uint16_t GetAreaSize(uint8_t area_index);
    uint16_t GetWrittenBytes(uint8_t area_index);
    uint16_t GetNumAreas(void);
    uint16_t GetMaximumAreaSize(void);
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
    uint16_t maximum_area_size                       = 0;
    std::unique_ptr<SharedMemory> _shared_memory_array[SharedMemoryManager::_maximum_shared_memory];

   public:
    /**
     * @brief Writes protobuf data to a specific memory area.
     *
     * This function writes protobuf data to a memory area specified by the area index.
     * It ensures that the index is within the valid range and that the memory area is not null.
     *
     * @tparam T The type of the protobuf data to write.
     * @param[in] area_index The index of the memory area to write to.
     * @param[in] protobuf A reference to the protobuf data to write.
     * @param[in] msg_desc A reference to the protobuf message descriptor.
     *
     * @return titan_err_t Result of the write operation:
     *         - ESP_OK on success.
     *         - Error::INVALID_MEMORY_AREA if the area index is out of bounds.
     *         - Error::NULL_PTR if the memory area is null.
     *         - Error::UNKNOW_FAIL if any other failure occurred.
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

    /**
     * @brief Writes a buffer to a specific memory area.
     *
     * This function writes raw buffer data to a memory area specified by the area index.
     * It checks that the index is valid, the memory area is not null, and the buffer fits
     * in the available memory space.
     *
     * @param[in] area_index The index of the memory area to write to.
     * @param[in] buffer The buffer containing the data to write.
     * @param[in] written_bytes The number of bytes to write from the buffer.
     *
     * @return titan_err_t Result of the write operation:
     *         - ESP_OK on success.
     *         - Error::INVALID_MEMORY_AREA if the area index is out of bounds.
     *         - Error::NULL_PTR if the memory area is null.
     *         - Error::BUFFER_OUT_OF_SPACE if the buffer size exceeds the memory area's size.
     *         - Error::UNKNOW_FAIL if any other failure occurred.
     */
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
    /**
     * @brief Reads protobuf data from a specific memory area.
     *
     * This function reads protobuf data from a memory area specified by the area index.
     * It checks that the index is valid and the memory area is not null.
     *
     * @tparam T The type of the protobuf data to read.
     * @param[in] area_index The index of the memory area to read from.
     * @param[out] protobuf A reference to the protobuf data to read into.
     * @param[in] msg_desc A reference to the protobuf message descriptor.
     * @param[in] silent If true, suppresses error messages.
     *
     * @return uint16_t The number of bytes read from the memory area.
     */
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

    /**
     * @brief Reads a buffer from a specific memory area.
     *
     * This function reads raw buffer data from a memory area specified by the area index.
     * It checks that the index is valid, the memory area is not null, and the buffer size is
     * sufficient to hold the data.
     *
     * @param[in] area_index The index of the memory area to read from.
     * @param[out] buffer The buffer to read the data into.
     * @param[in] buffer_size The size of the buffer.
     * @param[in] silent If true, suppresses error messages.
     *
     * @return uint16_t The number of bytes read from the memory area.
     */
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
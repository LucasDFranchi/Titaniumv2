#include "./MemoryAreas/MemoryArea.hpp"
#include "esp_err.h"
#include "stdint.h"

#ifndef MEMORY_MANAGER_GUARD
#define MEMORY_MANAGER_GUARD

/**
 * Manages memory allocation and access for a system.
 */
class MemoryManager {
   public:
    MemoryManager(const MemoryManager& obj) = delete;

    esp_err_t Initialize(void);
    bool IsAreaDataNew(area_index_e area_index);
    static MemoryManager* GetInstance(void);

   private:
    MemoryManager(){};
    uint8_t memory_area_array_size;
    MemoryArea* memory_area_array[AREAS_COUNT];
    static MemoryManager* singleton_pointer_;

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
    esp_err_t Write(area_index_e area_index, uint16_t size, T* pointer_data) {
        esp_err_t result = ESP_FAIL;

        do {
            if (pointer_data == nullptr) {
                result = ESP_ERR_NO_MEM;
                break;
            }

            if (area_index > AREAS_COUNT) {
                result = ESP_ERR_INVALID_ARG;
                break;
            }
            if (this->memory_area_array[area_index]->GetSize() < size) {
                result = ESP_ERR_INVALID_SIZE;
                break;
            }

            result = this->memory_area_array[area_index]->Write(
                reinterpret_cast<uint8_t*>(pointer_data), size);

        } while (0);

        return result;
    }
    /**
     * Reads data from a specific memory area.
     *
     * @param[in] area_index The index of the memory area to read from.
     * @param[out] size_pointer Amount of bytes read by the memory manager.
     * @param[in] pointer_data Pointer to the buffer where the read data will be
     * stored.
     *
     * @returns ESP_OK if the read operation is successful, otherwise an error
     * code.
     */
    template <typename T>
    uint16_t Read(area_index_e area_index, T* pointer_data) {
        uint16_t result = 0;

        do {
            if (area_index > AREAS_COUNT) {
                break;
            }
            if (this->memory_area_array[area_index]->Read(
                    reinterpret_cast<uint8_t*>(pointer_data)) != ESP_OK) {
                break;
            }

            result = this->memory_area_array[area_index]->GetSize();

        } while (0);

        return result;
    }
};

#endif /* MEMORY_MANAGER_GUARD */
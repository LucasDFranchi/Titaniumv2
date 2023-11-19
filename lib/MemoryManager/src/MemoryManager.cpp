#include "MemoryManager.h"
#include "./MemoryAreas/src/MemoryAreaDisplay.h"
#include "./MemoryAreas/src/MemoryAreaSSID.h"
#include "./MemoryAreas/src/MemoryAreaPassword.h"
#include "./MemoryAreas/src/MemoryAreaConnection.h"
#include "string.h"
#include <cstdio>

/**
 * Initializes the MemoryManager.
 *
 * @returns ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t MemoryManager::Initialize(void){
    this->memory_area_array[0] = new MemoryAreaDisplay;
    this->memory_area_array[1] = new MemoryAreaSSID;
    this->memory_area_array[2] = new MemoryAreaPassword;
    this->memory_area_array[3] = new MemoryAreaConnection;
    for (int i = 0; i < AREAS_COUNT; i++){
        this->memory_area_array[i]->Initialize();
    }

    return ESP_OK;
}

/**
 * Writes data to a specific memory area.
 *
 * @param[in] area_index The index of the memory area to write to.
 * @param[in] size The size of the data to write.
 * @param[in] pIn A pointer to the data to write.
 *
 * @returns An esp_err_t indicating the result of the write operation.
 *          - ESP_OK if the write operation was successful.
 *          - ESP_FAIL if an error occurred during the write operation.
 */
esp_err_t MemoryManager::Write(area_index_e area_index, uint32_t size, uint8_t *pIn){
    esp_err_t result = ESP_FAIL;

    do 
    {
        if (pIn == nullptr){
            result = ESP_ERR_NO_MEM;
            break;
        }
        if (area_index > AREAS_COUNT){
            result = ESP_ERR_INVALID_ARG;
            break;
        }
        if (this->memory_area_array[area_index]->GetAccess() == READ_ONLY){
            result = ESP_ERR_INVALID_STATE;
            break;
        }
        if (this->memory_area_array[area_index]->GetSize() < size){
            result = ESP_ERR_INVALID_SIZE;
            break;
        }

        result = this->memory_area_array[area_index]->Write(pIn, size);

    }while (0);

    return result;
}

/**
 * Reads data from a specific memory area.
 *
 * @param[in] area_index The index of the memory area to read from.
 * @param[out] size_pointer Amount of bytes read by the memory manager.
 * @param[in] pOut Pointer to the buffer where the read data will be stored.
 *
 * @returns ESP_OK if the read operation is successful, otherwise an error code.
 */
esp_err_t MemoryManager::Read(area_index_e area_index, uint16_t *size_pointer, uint8_t *pOut){
    esp_err_t result = ESP_FAIL;
    
    do 
    {
        if (pOut == nullptr){
            result = ESP_ERR_NO_MEM;
            break;
        }
        if (area_index > AREAS_COUNT){
            result = ESP_ERR_INVALID_ARG;
            break;
        }
        if (this->memory_area_array[area_index]->GetAccess() == WRITE_ONLY){
            result = ESP_ERR_INVALID_STATE;
            break;
        }

        *size_pointer = this->memory_area_array[area_index]->GetSize();
        result = this->memory_area_array[area_index]->Read(pOut);

    }while (0);

    return result;
}

/**
 * @brief Returns the singleton instance
 * 
 * @return MemoryManager* 
 */
MemoryManager* MemoryManager::GetInstance(void)
  {
    if (singleton_pointer_ == nullptr)
    {
      singleton_pointer_ = new MemoryManager();
    }

    return singleton_pointer_;
  }
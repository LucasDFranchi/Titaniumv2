#include "MemoryManager.h"
#include "./MemoryAreas/src/MemoryAreaDisplay.h"
#include "string.h"
#include <cstdio>

/**
 * Initializes the MemoryManager.
 *
 * @returns ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t MemoryManager::Initialize(void){
    this->memory_area_array[0] = new MemoryAreaDisplay;
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
            printf("pIn is a null pointer! \n");
            break;
        }
        if (area_index > AREAS_COUNT){
            printf("Area index is greater than expected! \n");
            break;
        }
        
        if (this->memory_area_array[area_index]->GetAccess() == READ_ONLY){
            printf("Area memory is read-only type! \n");
            break;
        }
        if (this->memory_area_array[area_index]->GetSize() != size){
            printf("Area size different than expected on Write: %ld != %ld! \n", this->memory_area_array[area_index]->GetSize(), size);
            break;
        }

        this->memory_area_array[area_index]->Write(pIn);

        result = ESP_OK;

    }while (0);

    return result;
}

/**
 * Reads data from a specific memory area.
 *
 * @param[in] area_index The index of the memory area to read from.
 * @param[in] size The size of the data to read.
 * @param[in] pOut Pointer to the buffer where the read data will be stored.
 *
 * @returns ESP_OK if the read operation is successful, otherwise an error code.
 */
esp_err_t MemoryManager::Read(area_index_e area_index, uint32_t size, uint8_t *pOut){
    esp_err_t result = ESP_FAIL;
    
    do 
    {
        if (pOut == nullptr){
            printf("pOut is a null pointer! \n");
            break;
        }
        if (area_index > AREAS_COUNT){
            printf("Area index is greater than expected! \n");
            break;
        }

        if (this->memory_area_array[area_index]->GetAccess() == WRITE_ONLY){
            printf("Area memory is write-only type! \n");
            break;
        }
        if (this->memory_area_array[area_index]->GetSize() != size){
            printf("Area size different than expected on Read: %ld != %ld! \n", this->memory_area_array[area_index]->GetSize(), size);
            break;
        }
        this->memory_area_array[area_index]->Read(pOut);

        result = ESP_OK;

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
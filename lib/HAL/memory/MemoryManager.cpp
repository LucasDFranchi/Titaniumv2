#include "MemoryManager.h"
#include "memory/AreaDefinitions/ConnectionArea.h"
#include "memory/AreaDefinitions/CredentialsArea.h"
#include "memory/AreaDefinitions/DisplayArea.h"
#include "memory/AreaDefinitions/LoRaReadArea.h"
#include "memory/AreaDefinitions/LoRaWriteArea.h"

constexpr AreaDef memory_area_def[AREAS_COUNT]{
    {READ_WRITE, DISPLAY_AREA, sizeof(Display::data_st), 0},
    {READ_WRITE, CONNECTION_AREA, sizeof(Connection::data_st), 0},
    {READ_WRITE, LORA_READ_AREA, sizeof(LoRaRead::data_st), 0},
    {READ_WRITE, LORA_WRITE_AREA, sizeof(LoRaWrite::data_st), 0},
    {READ_WRITE, CREDENTIALS_AREA, sizeof(Credentials::data_st), 0},
};

/**
 * Initializes the MemoryManager.
 *
 * @returns ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t MemoryManager::Initialize(void){

    for (int i = 0; i < AREAS_COUNT; i++){
        this->memory_area_array[i] = new MemoryArea(memory_area_def[i]);
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
esp_err_t MemoryManager::Write(area_index_e area_index, uint16_t size, IArea &area_data){
    esp_err_t result = ESP_FAIL;

    do 
    {
        if (area_index > AREAS_COUNT){
            result = ESP_ERR_INVALID_ARG;
            break;
        }
        if (this->memory_area_array[area_index]->GetSize() < size){
            result = ESP_ERR_INVALID_SIZE;
            break;
        }

        result = this->memory_area_array[area_index]->Write(reinterpret_cast<uint8_t*>(&area_data.raw_data), size);

    } while (0);

    return result;
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
esp_err_t MemoryManager::Write(area_index_e area_index, uint16_t size, uint8_t* data_pointer){
    esp_err_t result = ESP_FAIL;

    do 
    {
        if (area_index > AREAS_COUNT){
            result = ESP_ERR_INVALID_ARG;
            break;
        }
        if (this->memory_area_array[area_index]->GetSize() < size){
            result = ESP_ERR_INVALID_SIZE;
            break;
        }

        result = this->memory_area_array[area_index]->Write(data_pointer, size);

    } while (0);

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
uint16_t MemoryManager::Read(area_index_e area_index, IArea &area_data){
    uint16_t result = 0;
    
    do 
    {
        if (area_index > AREAS_COUNT){
            break;
        }
        if (this->memory_area_array[area_index]->Read(reinterpret_cast<uint8_t*>(&area_data.raw_data)) != ESP_OK){
            break;
        }

        result = this->memory_area_array[area_index]->GetSize();

    } while (0);

    return result;
}

/**
 * Reads data from a specific memory area.
 *
 * @param[in]  area_index The index of the memory area to read from.
 * @param[out] size_pointer Amount of bytes read by the memory manager.
 * @param[in]  pOut Pointer to the buffer where the read data will be stored.
 *
 * @returns ESP_OK if the read operation is successful, otherwise an error code.
 */
uint16_t MemoryManager::Read(area_index_e area_index, uint8_t* pointer_data){
    uint16_t result = 0;
    
    do 
    {
        if (area_index > AREAS_COUNT){
            break;
        }
        if (this->memory_area_array[area_index]->Read(pointer_data) != ESP_OK){
            break;
        }

        result = this->memory_area_array[area_index]->GetSize();

    } while (0);

    return result;
}

bool MemoryManager::IsAreaDataNew(area_index_e area_index){
    return this->memory_area_array[area_index]->GetAreaHasUpdated();
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
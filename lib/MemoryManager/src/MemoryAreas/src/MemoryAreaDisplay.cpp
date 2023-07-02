#include "MemoryAreaDisplay.h"
#include "string.h"

// #include <freertos/task.h>
// #include "freertos/semphr.h"


/**
 * Initializes the MemoryAreaDisplay object.
 *
 * @returns ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t MemoryAreaDisplay::Initialize(void){
    this->index = DISPLAY_AREA;
    this->size = 1024;
    this->access_type = READ_WRITE;
    this->cached_time = 0;
    this->has_update = false;
    this->mutex_ = xSemaphoreCreateMutex();
    memset(this->data, 0, this->size);

    return ESP_OK;
}


/**
 * Clears the memory area by setting all its data to zero.
 *
 * @returns ESP_OK if the memory area is successfully cleared.
 */
esp_err_t MemoryAreaDisplay::Clear(void){
    memset(this->data, 0, this->size);
    return ESP_OK;
}


/**
 * Retrieves the size of the memory area.
 *
 * @returns The size of the memory area.
 */
uint32_t MemoryAreaDisplay::GetSize(void){
    return this->size;
}


/**
 * Retrieves the access type of the memory area display.
 *
 * @returns The access type of the memory area display.
 */
access_type_e MemoryAreaDisplay::GetAccess(void){
    return this->access_type;
}


/**
 * Retrieves the index of the memory area.
 *
 * @returns The index of the memory area.
 */
area_index_e MemoryAreaDisplay::GetAreaIndex(void){
    return this->index;
}


/**
 * Writes data to the memory area display.
 *
 * @param pIn Pointer to the input data.
 *
 * @returns ESP_OK if the write operation is successful, otherwise an error code.
 */
esp_err_t MemoryAreaDisplay::Write(uint8_t *pIn){
    if( this->mutex_ != NULL )
    {
        if(xSemaphoreTake( this->mutex_, portMAX_DELAY ))
        {
            memcpy(this->data, pIn, this->size);
            xSemaphoreGive( this->mutex_ );
        }
    }
    return ESP_OK;       
}


/**
 * Reads the contents of the MemoryAreaDisplay object into the provided buffer.
 *
 * @param pOut Pointer to the buffer where the data will be copied to.
 *
 * @returns ESP_OK if the read operation is successful, otherwise an error code.
 */
esp_err_t MemoryAreaDisplay::Read(uint8_t *pOut){
    if( this->mutex_ != NULL )
    {
        if(xSemaphoreTake( this->mutex_, portMAX_DELAY ))
        {
            memcpy(pOut, this->data, this->size);
            xSemaphoreGive( this->mutex_ );
        }
    }

    return ESP_OK;      
}
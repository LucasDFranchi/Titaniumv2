#include "esp_err.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/semphr.h"

#include "MemoryAreaEnum.h"
#include "MemoryUtils.h"

#ifndef MEMORY_AREA_TEMPLATE_GUARD
#define MEMORY_AREA_TEMPLATE_GUARD

/**
 * Template for a memory area.
 */
class MemoryAreaTemplate {
    public:
    virtual esp_err_t Initialize(void) = 0;
    /**
     * Clears the memory area by setting all its data to zero.
     *
     * @returns ESP_OK if the memory area is successfully cleared.
     */
    esp_err_t Clear(void){
        return memset_s(this->data, 0, this->size);
    }
    /**
     * Retrieves the size of the memory area.
     *
     * @returns The size of the memory area.
     */
    uint32_t GetSize(void){
        return this->size;
    }
    
    /**
     * Retrieves the access type of the memory area display.
     *
     * @returns The access type of the memory area display.
     */
    access_type_e GetAccess(void){
        return this->access_type;
    }

    /**
     * Retrieves the index of the memory area.
     *
     * @returns The index of the memory area.
     */
    area_index_e GetAreaIndex(void){
        return this->index;
    }

    /**
     * Retrieves if an area was read.
     *
     * @returns Returns if the the area has been read already.
     */
    bool GetAreaHasUpdated(void){
        return this->has_update;
    }

    /**
     * Writes data to the memory area display.
     *
     * @param pIn Pointer to the input data.
     *
     * @returns ESP_OK if the write operation is successful, otherwise an error code.
     */
    esp_err_t Write(uint8_t *pIn, size_t size){
        auto result = ESP_FAIL;

        if( this->mutex_ != NULL )
        {
            if(xSemaphoreTake( this->mutex_, portMAX_DELAY ))
            {
                result = memcpy_s(this->data, pIn, size);
                this->has_update = true;

                xSemaphoreGive( this->mutex_ );
            }
        }
        return result;       
    }

    /**
     * Reads the contents of the MemoryAreaDisplay object into the provided buffer.
     *
     * @param pOut Pointer to the buffer where the data will be copied to.
     *
     * @returns ESP_OK if the read operation is successful, otherwise an error code.
     */
    esp_err_t Read(uint8_t *pOut){
        auto result = ESP_FAIL;

        if( this->mutex_ != NULL )
        {
            if(xSemaphoreTake( this->mutex_, portMAX_DELAY ))
            {   
                result = memcpy_s(pOut, this->data, this->size);
                this->has_update = false;
                xSemaphoreGive( this->mutex_ );
            }
        }

        return result;      
    }
    protected:
        area_index_e index;
        access_type_e access_type;
        uint8_t *data;
        uint8_t has_update;
        uint32_t size;
        uint32_t cached_time;
        SemaphoreHandle_t mutex_ = nullptr;
};

#endif /* MEMORY_AREA_TEMPLATE_GUARD */
#include "memory/AreaDefinitions/AreaDefinitions.h"
#include "MemoryUtils.h"

#include "esp_err.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/semphr.h"

#ifndef MEMORY_AREA_GUARD
#define MEMORY_AREA_GUARD

/**
 * Template for a memory area.
 */
class MemoryArea {
    public:
    MemoryArea(const AreaDef& area_definitions) {
        this->index = area_definitions.index;
        this->data = reinterpret_cast<IArea*>(operator new(area_definitions.size));
        this->size = area_definitions.size;
        this->access_type = area_definitions.access_type;
        this->cached_time = area_definitions.cached_time;
        this->has_update = false;
        this->mutex_ = xSemaphoreCreateMutex();
    
        this->Clear();
    }
    /**
     * Clears the memory area by setting all its data to zero.
     *
     * @returns ESP_OK if the memory area is successfully cleared.
     */
    esp_err_t Clear(void){
        return memset_s(reinterpret_cast<uint8_t*>(&this->data->raw_data), 0, this->size);
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
     * Retrieves the area data pointer.
     *
     * @returns Returns the data area pointer.
     */
    IArea* GetData(void) const{
        return this->data;
    }

    /**
     * Writes data to the memory area display.
     *
     * @param pIn Pointer to the input data.
     *
     * @returns ESP_OK if the write operation is successful, otherwise an error code.
     */
    esp_err_t Write(uint8_t* data_pointer, size_t size){
        auto result = ESP_FAIL;

        if (this->access_type == READ_ONLY){
            return result;
        }

        if( this->mutex_ != NULL )
        {
            if(xSemaphoreTake( this->mutex_, portMAX_DELAY ))
            {
                result = memcpy_s(reinterpret_cast<uint8_t*>(&this->data->raw_data), data_pointer, size);
                this->has_update = true;

                xSemaphoreGive( this->mutex_ );
            }
        }
        return result;       
    }

    /**
     * Reads the contents of the MemoryAreaDisplay object into the provided buffer.
     *
     * @param data_pointer Pointer to the buffer where the data will be copied to.
     *
     * @returns ESP_OK if the read operation is successful, otherwise an error code.
     */
    esp_err_t Read(uint8_t* data_pointer){
        auto result = ESP_FAIL;

        if (this->access_type == WRITE_ONLY){
            return result;
        }

        if( this->mutex_ != NULL )
        {
            if(xSemaphoreTake( this->mutex_, portMAX_DELAY ))
            {   
                result = memcpy_s(data_pointer, reinterpret_cast<uint8_t*>(&this->data->raw_data), this->size);
                this->has_update = false;
                xSemaphoreGive( this->mutex_ );
            }
        }

        return result;      
    }

    protected:
        area_index_e index;
        access_type_e access_type;
        IArea *data;
        uint8_t has_update;
        uint32_t size;
        uint32_t cached_time;
        SemaphoreHandle_t mutex_ = nullptr;
};

#endif /* MEMORY_AREA_GUARD */
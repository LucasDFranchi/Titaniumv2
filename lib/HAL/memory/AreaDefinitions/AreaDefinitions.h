
#include "stdint.h"
#include "MemoryUtils.h"

#ifndef AREA_DEFINITIONS_GUARD
#define AREA_DEFINITIONS_GUARD

/**
* Enumeration of access types for a resource.
*/
typedef enum access_type_e {
    READ_ONLY = 0,
    WRITE_ONLY,
    READ_WRITE,
}access_type_et;
/**
* Enumeration of area indices.
* 
* @enum area_index_e
*
* @constant DISPLAY_AREA The index of the Display area.,
* @constant SSID_AREA The index of the SSID WiFi area.,
* @constant CONNECTION_AREA The index of the Connection Status WiFi area.,
* @constant LORA_WRITE_AREA The index of the LoRa Write area.,
* @constant LORA_READ_AREA The index of the LoRa Read area.
* @constant AREAS_COUNT The total number of areas.
*/
typedef enum area_index_e {
    DISPLAY_AREA = 0,
    CONNECTION_AREA,
    LORA_READ_AREA,
    LORA_WRITE_AREA,
    CREDENTIALS_AREA,
    AREAS_COUNT,
}area_index_et;

class AreaDef {
public:
    constexpr AreaDef(access_type_et at, area_index_et ai, uint16_t sz, uint32_t ct) 
        : access_type(at), index(ai), size(sz), cached_time(ct) {
    }

    access_type_et access_type;
    area_index_et  index;
    uint16_t       size;
    uint32_t       cached_time;
};

class IArea {
public:
    virtual ~IArea() = default;

    typedef struct data_s{
        uint8_t field;
    }data_st;

    data_st raw_data;
};

#endif /* AREA_DEFINITIONS_GUARD  */
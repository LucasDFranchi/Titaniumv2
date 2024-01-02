#include "AreaDefinitions.h"

#ifndef LORA_READ_AREA_GUARD
#define LORA_READ_AREA_GUARD

struct LoRaRead : public IArea{

    LoRaRead() {
        memset_s(reinterpret_cast<uint8_t*>(&raw_data), 0, sizeof(raw_data));
    };

    typedef struct data_s{
        uint8_t rx_buffer[64];
    } __attribute__((packed)) data_st;

    data_st raw_data;
    
};

#endif /* LORA_READ_AREA_GUARD  */
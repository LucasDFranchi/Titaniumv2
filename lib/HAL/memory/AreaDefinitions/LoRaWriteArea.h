#include "AreaDefinitions.h"

#ifndef LORA_WRITE_AREA_GUARD
#define LORA_WRITE_AREA_GUARD

struct LoRaWrite : public IArea{

    LoRaWrite() {
        memset_s(reinterpret_cast<uint8_t*>(&raw_data), 0, sizeof(raw_data));
    };

    typedef struct data_s{
        uint8_t tx_buffer[64];
    } __attribute__((packed)) data_st;

    data_st raw_data;
};

#endif /* LORA_WRITE_AREA_GUARD  */
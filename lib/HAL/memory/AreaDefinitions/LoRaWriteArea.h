#include "AreaDefinitions.h"

#ifndef LORA_WRITE_AREA_GUARD
#define LORA_WRITE_AREA_GUARD

typedef struct lora_write_s{
    uint8_t tx_buffer[64];
} __attribute__((packed)) lora_write_st;

#endif /* LORA_WRITE_AREA_GUARD  */
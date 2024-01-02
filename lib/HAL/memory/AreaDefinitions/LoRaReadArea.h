#include "AreaDefinitions.h"

#ifndef LORA_READ_AREA_GUARD
#define LORA_READ_AREA_GUARD

typedef struct lora_read_s{
    uint8_t rx_buffer[64];
} __attribute__((packed)) lora_read_st;

#endif /* LORA_READ_AREA_GUARD  */
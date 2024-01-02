#include "AreaDefinitions.h"

#ifndef CREDENTIALS_AREA_GUARD
#define CREDENTIALS_AREA_GUARD

typedef struct credentials_s{
    uint8_t sta_ssid[32];
    uint8_t sta_password[64];
} __attribute__((packed)) credentials_st;

#endif /* CREDENTIALS_AREA_GUARD  */
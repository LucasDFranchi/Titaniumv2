#include "AreaDefinitions.h"

#ifndef CREDENTIALS_AREA_GUARD
#define CREDENTIALS_AREA_GUARD

typedef struct credentials_s{
    char sta_ssid[32];
    char sta_password[64];
} __attribute__((packed)) credentials_st;

#endif /* CREDENTIALS_AREA_GUARD  */
#include "AreaDefinitions.h"

#ifndef CONNECTION_AREA_GUARD
#define CONNECTION_AREA_GUARD

typedef struct connection_s{
    uint8_t connection_ap_status;
    uint8_t connection_sta_status;
} __attribute__((packed)) connection_st;

#endif /* CONNECTION_AREA_GUARD  */
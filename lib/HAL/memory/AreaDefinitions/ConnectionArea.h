#include "AreaDefinitions.h"

#ifndef CONNECTION_AREA_GUARD
#define CONNECTION_AREA_GUARD

struct Connection : public IArea{

    Connection() {
        memset_s(reinterpret_cast<uint8_t*>(&raw_data), 0, sizeof(raw_data));
    };
    
    typedef struct data_s{
        uint8_t connection_status;
    } __attribute__((packed)) data_st;

    data_st raw_data;
};

#endif /* CONNECTION_AREA_GUARD  */
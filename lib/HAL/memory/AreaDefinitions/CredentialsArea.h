#include "AreaDefinitions.h"

#ifndef CREDENTIALS_AREA_GUARD
#define CREDENTIALS_AREA_GUARD

struct Credentials : public IArea{

    Credentials() {
        memset_s(reinterpret_cast<uint8_t*>(&raw_data), 0, sizeof(raw_data));
    };

    typedef struct data_s{
        uint8_t sta_ssid[32];
        uint8_t sta_password[64];
    } __attribute__((packed)) data_st;

    data_st raw_data;

};

#endif /* CREDENTIALS_AREA_GUARD  */
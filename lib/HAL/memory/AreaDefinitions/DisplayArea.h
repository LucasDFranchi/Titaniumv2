#include "AreaDefinitions.h"

#ifndef DISPLAY_AREA_GUARD
#define DISPLAY_AREA_GUARD

struct Display : public IArea{

    Display() {
        memset_s(reinterpret_cast<uint8_t*>(&raw_data), 0, sizeof(raw_data));
    };

    typedef struct data_s{
        uint8_t pixels[1024];
    } __attribute__((packed)) data_st;

    data_st raw_data;
};

#endif /* DISPLAY_AREA_GUARD  */
#include "AreaDefinitions.h"

#ifndef DISPLAY_AREA_GUARD
#define DISPLAY_AREA_GUARD

typedef struct display_s{
    uint8_t pixels[1024];
} __attribute__((packed)) display_st;

#endif /* DISPLAY_AREA_GUARD  */
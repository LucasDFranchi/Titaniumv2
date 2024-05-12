#ifndef GRAPHIC_DRIVER_INTERFACE_GUARD
#define GRAPHIC_DRIVER_INTERFACE_GUARD

#include "esp_err.h"

class IGraphicDriver {
    public:
    virtual esp_err_t Initialize(void) = 0;
    virtual void Process(uint8_t *memory_area_data) = 0;
};

#endif /* GRAPHIC_DRIVER_INTERFACE_GUARD */
#ifndef GRAPHIC_DRIVER_INTERFACE_GUARD
#define GRAPHIC_DRIVER_INTERFACE_GUARD

#include "esp_err.h"

class IGraphicDriver {
    public:
    virtual esp_err_t Init(void) = 0;
    virtual void UpdatePage(void) = 0;
};

#endif /* GRAPHIC_DRIVER_INTERFACE_GUARD */
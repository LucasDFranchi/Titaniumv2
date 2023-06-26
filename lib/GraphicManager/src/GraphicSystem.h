#include "./Drivers/IGraphicDriver.h"

#ifndef GRAPHIC_SYSTEM_GUARD
#define GRAPHIC_SYSTEM_GUARD

class GraphicSystem {
    public:
    void Init(void);
    void Loop_(void);
    private:
    IGraphicDriver *driver;
};

#endif /* GRAPHIC_SYSTEM_GUARD */
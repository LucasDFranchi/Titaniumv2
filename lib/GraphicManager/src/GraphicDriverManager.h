#include "ProcessTemplate.h"
#include "./Drivers/IGraphicDriver.h"

#ifndef GRAPHIC_DRIVER_MANAGER_GUARD
#define GRAPHIC_DRIVER_MANAGER_GUARD

/**
 * A class that manages the graphic driver process.
 */
class GraphicDriverManager : public ProcessTemplate{
    public:
    GraphicDriverManager() : ProcessTemplate(this, "Graphic Proccess", 10240, 1, &this->process_handler) { };
    void Execute(void);
    private:
    TaskHandle_t process_handler = NULL;
};

#endif /* GRAPHIC_DRIVER_MANAGER_GUARD */
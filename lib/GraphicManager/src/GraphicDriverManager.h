#include "ProcessTemplate.h"
#include "memory/MemoryManager.hpp"
#include "./Drivers/IGraphicDriver.h"

#include "memory/AreaDefinitions/DisplayArea.h"

#ifndef GRAPHIC_DRIVER_MANAGER_GUARD
#define GRAPHIC_DRIVER_MANAGER_GUARD

/**
 * A class that manages the graphic driver process.
 */
class GraphicDriverManager : public ProcessTemplate{
    public:
    GraphicDriverManager(const char* name, uint32_t memory, UBaseType_t  priority) : ProcessTemplate(this, name, memory, priority, &this->process_handler_) { };
    void Execute(void);
    
    private:
    esp_err_t Initialize_(void);
    
    private:
    MemoryManager*  memory_manager = nullptr;
    IGraphicDriver* graphic_driver = nullptr;
    display_st      display_buffer_;
    TaskHandle_t    process_handler_ = NULL;
};

#endif /* GRAPHIC_DRIVER_MANAGER_GUARD */
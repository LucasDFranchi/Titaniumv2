#include "GraphicSystem.h"
#include "./Drivers/SSD1306.h"

void GraphicSystem::Init(void){
    this->driver = new SSD1306;

    if (this->driver->Init() != ESP_OK)
    {
        return;
    }

    this->Loop_();
}

void GraphicSystem::Loop_(void){
    while(1){
        this->driver->UpdatePage();
    }
}
#include "GraphicSystem.h"
#include "./Drivers/SSD1306.h"

void GraphicSystem::Init(void){
    this->driver = new SSD1306;
}
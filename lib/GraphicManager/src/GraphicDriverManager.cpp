#include "GraphicDriverManager.h"
#include "MemoryManager.h"
#include "./Drivers/SSD1306.h"

/**
 * Executes the process of the GraphicDriverManager.
 * This function initializes the SSD1306 driver, reads data from the memory manager,
 * and processes the data using the driver. It runs in an infinite loop with a delay of 100ms between iterations.
 *
 * @returns None
 */
void GraphicDriverManager::Execute(void)
{
    auto driver = new SSD1306;
    auto memory_manager = MemoryManager::GetInstance();
    uint8_t memory_area_data[1024] = {0};

    if (driver->Initialize() != ESP_OK)
    {
        gpio_reset_pin(GPIO_NUM_25);
        gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
        gpio_set_level(GPIO_NUM_25, 1);
        return;
    }

    while(1){
        memory_manager->Read(DISPLAY_AREA, 1024, memory_area_data);
        driver->Process(memory_area_data);
        vTaskDelay(pdMS_TO_TICKS(150));
    }
}
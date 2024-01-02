#include "GraphicDriverManager.h"
#include "./Drivers/SSD1306.h"
#include "MemoryUtils.h"

esp_err_t GraphicDriverManager::Initialize_(void){
    auto result = ESP_FAIL;

    this->graphic_driver = new SSD1306;
    this->memory_manager = MemoryManager::GetInstance();
    result = this->graphic_driver->Initialize();

    return result;
}

/**
 * @brief Executes the GraphicDriverManager process.
 *
 * This function initializes the SSD1306 driver, reads data from the MemoryManager,
 * and processes the data using the driver. It runs in an infinite loop with a delay
 * of 150 milliseconds between iterations.
 *
 * @return None.
 */
void GraphicDriverManager::Execute(void)
{

    if (this->Initialize_() != ESP_OK){
        vTaskDelete(this->process_handler_);
    }

    while(1){
        memory_manager->Read(DISPLAY_AREA, this->display_buffer_);
        this->graphic_driver->Process(this->display_buffer_.raw_data.pixels);
        vTaskDelay(pdMS_TO_TICKS(150));
    }
}
#include "nvs_flash.h"

#include <HAL/memory/SharedMemoryManager.h>
#include <HAL/gpio/GPIOManager.h>
#include <HAL/spi/SPIManager.h>
// #include <gpio/GPIOManager.h>
// #include <spi/SPIManager.h>

// #include <CommandManager.h>
// #include <GraphicDriverManager.h>
// #include <SerialDriverManager.h>
// #include <LoRaManager.h>
// #include <NetworkManager.h>
// #include <HTTPServerManager.h>

SharedMemoryManager* SharedMemoryManager::singleton_pointer_ = nullptr;
GPIOManager*         GPIOManager::singleton_pointer_ = nullptr;
SPIManager*          SPIManager::singleton_pointer_ = nullptr;

esp_err_t initialize_nvs(void)
{
    nvs_flash_erase();
    esp_err_t ret = nvs_flash_init();
    return ret;
}

int main(void)
{
  initialize_nvs();

  auto shared_memory_manager = SharedMemoryManager::GetInstance();
  auto gpio_manager = GPIOManager::GetInstance();
  auto spi_manager = SPIManager::GetInstance();

  shared_memory_manager->Initialize();
  gpio_manager->Initialize();
  spi_manager->Initialize();

  while(1){
    gpio_manager->WriteGPIO(LED_WHITE, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_manager->WriteGPIO(LED_WHITE, LOW);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }

  // gpio_manager->WriteGPIO(LED_WHITE, HIGH);

  // auto serial_manager = new SerialDriverManager("Serial Proccess", 10240, 5);
  // auto network_manager = new NetworkManager("Network Proccess", 10240, 4);
  // auto graphic_manager = new GraphicDriverManager("Graphic Process", 2048*2, 1);
  // auto lora_manager = new LoRaManager("Lora Process", 3072, 2);
  // auto http_server_manager = new HTTPServerManager("HTTP Server Process", 10240*2, 2);
  
  // serial_manager->InitializeProcess();
  // network_manager->InitializeProcess();
  // graphic_manager->InitializeProcess();
  // lora_manager->InitializeProcess();
  // http_server_manager->InitializeProcess();
  
  return 0;
}

extern "C" void app_main(void) { main(); }
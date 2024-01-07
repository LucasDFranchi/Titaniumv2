#include "nvs_flash.h"

#include <memory/MemoryManager.hpp>
#include <gpio/GPIOManager.h>
#include <spi/SPIManager.h>

#include <CommandManager.h>
#include <GraphicDriverManager.h>
#include <SerialDriverManager.h>
#include <LoRaManager.h>
#include <NetworkManager.h>

MemoryManager* MemoryManager::singleton_pointer_ = nullptr;
GPIOManager*   GPIOManager::singleton_pointer_ = nullptr;
SPIManager*    SPIManager::singleton_pointer_ = nullptr;

esp_err_t initialize_nvs(void)
{
    nvs_flash_erase();
    esp_err_t ret = nvs_flash_init();
    return ret;
}

int main(void)
{
  initialize_nvs();

  auto memory_manager = MemoryManager::GetInstance();
  auto gpio_manager = GPIOManager::GetInstance();
  auto spi_manager = SPIManager::GetInstance();

  memory_manager->Initialize();
  gpio_manager->Initialize();
  spi_manager->Initialize();

  auto serial_manager = new SerialDriverManager("Serial Proccess", 10240, 5);
  auto network_manager = new NetworkManager("Network Proccess", 10240, 4);
  auto graphic_manager = new GraphicDriverManager("Graphic Process", 2048*2, 2);
  auto lora_manager = new LoRaManager("Lora Process", 3072, 1);
  
  serial_manager->InitializeProcess();
  network_manager->InitializeProcess();
  graphic_manager->InitializeProcess();
  lora_manager->InitializeProcess();

  
  return 0;
}

extern "C" void app_main(void) { main(); }
#include "nvs_flash.h"

#include <MemoryManager.h>
#include <GPIOManager.h>
#include <SPIManager.h>

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

  auto graphic_manager = new GraphicDriverManager;
  // auto network_manager = new NetworkManager;
  auto serial_manager = new SerialDriverManager;
  auto lora_manager = new LoRaManager;
  
  serial_manager->InitializeProcess();
  graphic_manager->InitializeProcess();
  // // network_manager->InitializeProcess();
  lora_manager->InitializeProcess();

  
  return 0;
}

extern "C" void app_main(void) { main(); }
#include "nvs_flash.h"

#include <CommandManager.h>
#include <MemoryManager.h>
#include <GraphicDriverManager.h>
#include <SerialDriverManager.h>
#include <NetworkManager.h>

MemoryManager* MemoryManager::singleton_pointer_ = nullptr;;

esp_err_t initialize_nvs(void)
{
    nvs_flash_erase();
    esp_err_t ret = nvs_flash_init();
    return ret;
}

int main(void)
{
  initialize_nvs();

  auto graphic_manager = new GraphicDriverManager;
  auto network_manager = new NetworkManager;
  auto memory_manager = MemoryManager::GetInstance();
  auto serial_manager = new SerialDriverManager;
  
  memory_manager->Initialize();

  graphic_manager->InitializeProcess();
  // network_manager->InitializeProcess();
  serial_manager->InitializeProcess();
  

  return 0;
}

extern "C" void app_main(void) { main(); }
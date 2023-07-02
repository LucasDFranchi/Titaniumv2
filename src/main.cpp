#include <CommandManager.h>
#include <MemoryManager.h>
#include <GraphicDriverManager.h>
#include <SerialDriverManager.h>

CommandManager command_manager;
MemoryManager* MemoryManager::singleton_pointer_ = nullptr;;
auto command_blink = new CommandBlink(1000);

uint8_t page[1024] =  {0};

int main(void)
{
  auto graphic_manager = new GraphicDriverManager;
  auto serial_manager = new SerialDriverManager;
  auto memory_manager = MemoryManager::GetInstance();
  
  memory_manager->Initialize();
  memory_manager->Write(DISPLAY_AREA, 1024, page);

  graphic_manager->InitializeProcess();
  serial_manager->InitializeProcess();
  

  return 0;
}

extern "C" void app_main(void) { main(); }
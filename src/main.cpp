#include <CommandManager.h>
#include <GraphicSystem.h>

CommandManager command_manager;
auto command_blink = new CommandBlink(1000);

int main(void)
{
  GraphicSystem g;

  g.Init();
  

  return 0;
}

extern "C" void app_main(void) { main(); }
#include <CommandManager.h>
#include <GraphicSystem.h>

CommandManager command_manager;
auto command_blink = new CommandBlink(1000);

int main(void)
{
  GraphicSystem g;

  g.Init();

  while (1)
  {
    static bool ret = false;

    if (!ret)
      ret = command_manager.ExecuteCommand(command_blink);
  }

  return 0;
}

extern "C" void app_main(void) { main(); }
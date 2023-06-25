#include "ICommand.h"

#ifndef COMMAND_BLINK_GUARD
#define COMMAND_BLINK_GUARD

class CommandBlink : public ICommand
{
public:
    CommandBlink(int blink_time);
    void Setup(void);
    bool Loop(void);
    void Teardown(void);

private:
    int LED_BUILTIN = 25;
    int blink_time = 0;
    int counter = 0;
};

#endif /* COMMAND_BLINK_GUARD */
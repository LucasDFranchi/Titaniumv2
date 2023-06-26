#include "./Commands/ICommand.h"
#include "./Commands/CommandBlink.h"

#ifndef COMMAND_MANAGER_GUARD
#define COMMAND_MANAGER_GUARD

class CommandManager{
    public:
    CommandManager(void){
        this->command_state = 0;
    }
    bool ExecuteCommand(ICommand *command);
    private:
    int command_state;
};

#endif /* COMMAND_MANAGER_GUARD */
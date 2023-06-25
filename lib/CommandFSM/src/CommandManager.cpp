#include "CommandManager.h"

bool CommandManager::ExecuteCommand(ICommand *command)
{
    bool ret = false;

    if (command == nullptr)
    {
        return true;
    }

    switch (this->command_state)
    {
    case 0:
    {
        command->Setup();
        this->command_state = 1;
        break;
    }
    case 1:
    {
        if (command->Loop())
        {
            this->command_state = 2;
        }
        break;
    }
    case 2:
    default:
    {
        command->Teardown();
        this->command_state = 0;
        ret = true;
    }
    }

    return ret;
}
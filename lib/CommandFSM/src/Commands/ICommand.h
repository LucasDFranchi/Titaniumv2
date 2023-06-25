#ifndef COMMAND_INTERFACE_GUARD
#define COMMAND_INTERFACE_GUARD

class ICommand {
    public:
    virtual void Setup(void) = 0;
    virtual bool Loop(void) = 0;
    virtual void Teardown(void) = 0;
};

#endif /* COMMAND_INTERFACE_GUARD */
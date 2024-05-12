#ifndef TITANIUM_PROTOCOL_ENUMS_GUARD
#define TITANIUM_PROTOCOL_ENUMS_GUARD

#include "stdint.h"

enum command_e : uint8_t {
    INVALID_OPERATION = 0,
    READ_OPERATION    = 'R',
    WRITE_OPERATION   = 'W',
};

#endif /* TITANIUM_PROTOCOL_ENUMS_GUARD */
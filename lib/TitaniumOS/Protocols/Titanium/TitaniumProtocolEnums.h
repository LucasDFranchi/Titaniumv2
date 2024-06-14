#ifndef TITANIUM_PROTOCOL_ENUMS_GUARD
#define TITANIUM_PROTOCOL_ENUMS_GUARD

#include "stdint.h"

/**
 * @enum command_e
 * @brief Enum for defining command types in the Titanium protocol.
 */
enum command_e : uint8_t {
    INVALID_OPERATION = 0,   /**< Invalid operation command. */
    READ_OPERATION    = 'R', /**< Read operation command. */
    WRITE_OPERATION   = 'W', /**< Write operation command. */
};

#endif /* TITANIUM_PROTOCOL_ENUMS_GUARD */
#ifndef TITANIUM_PROTOCOL_ENUMS_GUARD
#define TITANIUM_PROTOCOL_ENUMS_GUARD

#include "stdint.h"

/**
 * @enum command_e
 * @brief Enum for defining command types in the Titanium protocol.
 */
enum command_e : uint8_t {
    INVALID_OPERATION = 0,   /**< Invalid command. */
    READ_COMMAND      = 'R', /**< Read command. */
    WRITE_COMMAND     = 'W', /**< Write command. */
    RESPONSE_COMMAND  = 'E', /**< Response command. */
};

#endif /* TITANIUM_PROTOCOL_ENUMS_GUARD */
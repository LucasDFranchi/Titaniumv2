#ifndef TITANIUM_PROTOCOL_ENUMS_H
#define TITANIUM_PROTOCOL_ENUMS_H

#include "stdint.h"

/**
 * @enum command_e
 * @brief Enum for defining command types in the Titanium protocol.
 */
enum command_e : uint8_t {
    INVALID_OPERATION            = 0, /**< Invalid command. */
    ACK_COMMAND                  = 1, /**< Ack command. */
    READ_COMMAND                 = 2, /**< Read command. */
    READ_RESPONSE_COMMAND        = 3, /**< Response command. */
    WRITE_COMMAND                = 4, /**< Write command. */
    READ_RESPONSE_SECURE_COMMAND = 101, /**< Response command and ask for ack. */
    WRITE_SECURE_COMMAND         = 102, /**< Write command and ask for ack. */
    READ_SECURE_COMMAND         = 103, /**< Read command and ask for ack. */
};

#endif /* TITANIUM_PROTOCOL_ENUMS_H */
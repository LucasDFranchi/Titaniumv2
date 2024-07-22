#ifndef TITANIUM_PROTOCOL_ENUMS_H
#define TITANIUM_PROTOCOL_ENUMS_H

#include "stdint.h"

/**
 * @enum command_e
 * @brief Enum for defining command types in the Titanium protocol.
 */
enum command_e : uint8_t {
    INVALID_OPERATION     = 0,   /**< Invalid command. */
    ACK_COMMAND           = 'A', /**< Ack command. */
    NACK_COMMAND          = 'N', /**< Ack command. */
    READ_COMMAND          = 'R', /**< Read command. */
    READ_RESPONSE_COMMAND = 'E', /**< Response command. */
    WRITE_COMMAND         = 'W', /**< Write command. */
};

#endif /* TITANIUM_PROTOCOL_ENUMS_H */
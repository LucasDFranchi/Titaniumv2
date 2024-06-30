#ifndef COMMUNICATION_PROCESS_DATA_MODEL_H
#define COMMUNICATION_PROCESS_DATA_MODEL_H

#include "Protocols/Titanium/TitaniumProtocolEnums.h"

typedef struct communication_request_s {
    uint8_t memory_area;
    command_e command;
} __attribute__((packed)) communication_request_st;

#endif /* COMMUNICATION_PROCESS_DATA_MODEL_H */

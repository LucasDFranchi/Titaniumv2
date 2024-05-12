#ifndef MANAGERS_AREA_INDEX
#define MANAGERS_AREA_INDEX

#include "stdint.h"

namespace ManagersAreaIndex {
    constexpr int8_t INVALID       = -1;
    constexpr int8_t SCRATCH       = 0;
    constexpr int8_t CREDENTIALS   = 1;
    constexpr int8_t CONNECTION    = 2;
    constexpr int8_t UART_RECEIVE  = 3;
    constexpr int8_t UART_TRANSMIT = 4;
    constexpr int8_t LORA_RECEIVE  = 5;
    constexpr int8_t LORA_TRANSMIT = 6;
}  // namespace ManagersAreaIndex

#endif /* MANAGERS_AREA_INDEX */
#ifndef MANAGERS_AREA_INDEX_H
#define MANAGERS_AREA_INDEX_H

#include "stdint.h"

/**
 * @brief Namespace containing constants for different manager area indexes.
 */
namespace ProcessAreaIndex {
    /**
     * @brief Invalid index.
     */
    constexpr int8_t INVALID = -1;

    /**
     * @brief Index for scratch area.
     */
    constexpr int8_t SCRATCH = 0;

    /**
     * @brief Index for credentials area.
     */
    constexpr int8_t CREDENTIALS = 1;

    /**
     * @brief Index for connection area.
     */
    constexpr int8_t CONNECTION = 2;

    /**
     * @brief Index for UART receive area.
     */
    constexpr int8_t UART_RECEIVE = 3;

    /**
     * @brief Index for UART transmit area.
     */
    constexpr int8_t UART_TRANSMIT = 4;

    /**
     * @brief Index for LoRa receive area.
     */
    constexpr int8_t LORA_RECEIVE = 5;

    /**
     * @brief Index for LoRa transmit area.
     */
    constexpr int8_t LORA_TRANSMIT = 6;
}  // namespace ProcessAreasIndex

#endif /* MANAGERS_AREA_INDEX_H */

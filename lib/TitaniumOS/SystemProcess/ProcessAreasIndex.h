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
     * @brief Index for UART transmit area.
     */
    constexpr int8_t UART_TRANSMIT = 3;

    /**
     * @brief Index for LoRa transmit area.
     */
    constexpr int8_t LORA_TRANSMIT = 4;
}  // namespace ProcessAreaIndex

/**
 * @brief Namespace containing constants for different manager area indexes.
 */
namespace CustomProcessAreaIndex {
    /**
     * @brief Invalid index.
     */
    constexpr int8_t WATER_LEVEL = 5;
}  // namespace CustomProcessAreaIndex


#endif /* MANAGERS_AREA_INDEX_H */

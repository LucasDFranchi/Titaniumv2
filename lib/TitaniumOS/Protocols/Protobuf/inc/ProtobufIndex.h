
#ifndef PROTOBUF_INDEX_H
#define PROTOBUF_INDEX_H

#include "stdint.h"

/**
 * @brief Namespace containing constants for different protobufs indexes.
 */
namespace ProtobufIndex {
    /**
    * @brief Invalid index.
    */
    constexpr int8_t INVALID = -1;

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
    constexpr int8_t UART_TX = 3;

    /**
     * @brief Index for UART received area.
     */
    constexpr int8_t UART_RX = 4;

    /**
     * @brief Index for LoRa transmit area.
     */
    constexpr int8_t LORA_TX = 5;

    /**
     * @brief Index for LoRa received area.
     */
    constexpr int8_t LORA_RX = 6;

    /**
     * @brief Index for MQTT uri area.
     */
    constexpr int8_t MQTT_URI = 7;

    /**
     * @brief None
     */
    constexpr int8_t WATER_LEVEL = 8;

    /**
    * @brief The index of the last memory area
    */
    constexpr int8_t LAST_MEMORY_AREA_INDEX = 8;

}  // namespace ProtobufIndex

#endif /* PROTOBUF_INDEX_H */

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
    constexpr int8_t CONNECTIONSTATUS = 2;

    /**
     * @brief Index for UART transmit area.
     */
    constexpr int8_t UART_TRANSMIT = 3;

    /**
     * @brief Index for UART received area.
     */
    constexpr int8_t UART_RECEIVE = 4;

    /**
     * @brief Index for LoRa transmit area.
     */
    constexpr int8_t LORA_TRANSMIT = 5;

    /**
     * @brief Index for LoRa received area.
     */
    constexpr int8_t LORA_RECEIVE = 6;

    /**
     * @brief Index for MQTT uri area.
     */
    constexpr int8_t MQTTURI = 7;

    /**
     * @brief Index for Water Level custom process area.
     */
    constexpr int8_t WATERLEVEL = 8;

    /**
    * @brief The index of the last memory area
    */
    constexpr int8_t LAST_MEMORY_AREA_INDEX = 8;

}  // namespace ProtobufIndex

#endif /* PROTOBUF_INDEX_H */
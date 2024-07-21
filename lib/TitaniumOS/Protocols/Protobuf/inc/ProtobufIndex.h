#ifndef MANAGERS_AREA_INDEX_H
#define MANAGERS_AREA_INDEX_H

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
    constexpr int8_t CREDENTIALS = 0;

    /**
     * @brief Index for connection area.
     */
    constexpr int8_t CONNECTION = 1;

    /**
     * @brief Index for UART transmit area.
     */
    constexpr int8_t UART_TRANSMIT = 2;

    /**
     * @brief Index for LoRa transmit area.
     */
    constexpr int8_t LORA_TRANSMIT = 3;

    /**
     * @brief Index for MQTT area.
     */
    constexpr int8_t MQTT = 4;

    /**
     * @brief Invalid index.
     */
    constexpr int8_t END_DEVICE_00 = 5;

    /**
     * @brief Invalid index.
     */
    constexpr int8_t END_DEVICE_01 = 6;

    /**
     * @brief Invalid index.
     */
    constexpr int8_t END_DEVICE_02 = 7;

    /**
     * @brief Invalid index.
     */
    constexpr int8_t END_DEVICE_03 = 8;

    /**
     * @brief Invalid index.
     */
    constexpr int8_t END_DEVICE_04 = 9;
}  // namespace ProtobufIndex


#endif /* PROTOBUF_INDEX_H */
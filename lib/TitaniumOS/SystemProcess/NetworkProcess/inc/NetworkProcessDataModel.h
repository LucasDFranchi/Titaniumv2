#ifndef NETWORK_MANAGER_DATA_MODEL_H
#define NETWORK_MANAGER_DATA_MODEL_H

#include "stdint.h"

/**
 * @brief Structure to hold Wi-Fi credentials.
 */
typedef struct credentials_s {
    uint8_t sta_ssid[32]; ///< SSID of the station (STA) mode Wi-Fi network.
    uint8_t sta_password[64]; ///< Password of the station (STA) mode Wi-Fi network.
} __attribute__((packed)) credentials_st;

/**
 * @brief Structure to hold connection status.
 */
typedef struct connection_s {
    uint8_t connection_ap_status; ///< Status of the access point (AP) connection.
    uint8_t connection_sta_status; ///< Status of the station (STA) connection.
} __attribute__((packed)) connection_st;

/**
 * @brief Namespace containing constants related to network status.
 */
namespace NetworkStatus {
    constexpr uint8_t NOT_CONNECTED = 0; ///< Indicates that the device is not connected to any network.
    constexpr uint8_t CONNECTED = 1;     ///< Indicates that the device is connected to a network.
}  // namespace NetworkStatus

#endif /* NETWORK_MANAGER_DATA_MODEL_H */

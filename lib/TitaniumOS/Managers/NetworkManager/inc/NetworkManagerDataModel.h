#ifndef NETWORK_MANAGER_DATA_MODEL
#define NETWORK_MANAGER_DATA_MODEL

#include "stdint.h"

/**
 * @brief Structure to hold Wi-Fi credentials.
 */
typedef struct credentials_s {
    uint8_t sta_ssid[32];
    uint8_t sta_password[64];
} __attribute__((packed)) credentials_st;

/**
 * @brief Structure to hold connection status.
 */
typedef struct connection_s {
    uint8_t connection_ap_status;
    uint8_t connection_sta_status;
} __attribute__((packed)) connection_st;

/**
 * @brief Namespace containing constants related to network status.
 */
namespace NetworkStatus {
    constexpr uint8_t NOT_CONNECTED = 0;
    constexpr uint8_t CONNECTED     = 1;
}  // namespace NetworkStatus

#endif /* NETWORK_MANAGER_DATA_MODEL */
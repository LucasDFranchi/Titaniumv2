#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include "Protocols/Protobuf/inc/titanium.pb.h"

/**
 * @brief Updates the connection status of the Access Point (AP).
 *
 * This function checks if the current access point status has changed compared to the last known status.
 * If a change is detected, it updates the last status with the current status.
 *
 * @param actual_status The current access point status.
 * @param last_status The previous access point status, which will be updated if a change is detected.
 * @return True if the status has changed, otherwise false.
 */
inline bool ShouldUpdateConnectionStatus(access_point_status *actual_status, access_point_status *last_status) {
    if ((actual_status == nullptr) || (last_status == nullptr)) {
        return false;
    }

    if (actual_status->status != last_status->status) {
        return false;
    }

    last_status->status = actual_status->status;

    return true;
}

/**
 * @brief Updates the connection status of the Station (STA).
 *
 * This function checks if the current station status has changed compared to the last known status.
 * If a change is detected, it updates the last status with the current status.
 *
 * @param actual_status The current station status.
 * @param last_status The previous station status, which will be updated if a change is detected.
 * @return True if the status has changed, otherwise false.
 */
inline bool ShouldUpdateConnectionStatus(station_status *actual_status, station_status *last_status) {
    if ((actual_status == nullptr) || (last_status == nullptr)) {
        return false;
    }

    if (actual_status->status != last_status->status) {
        return false;
    }

    last_status->status = actual_status->status;

    return true;
}

#endif /* NETWORK_UTILS_H */

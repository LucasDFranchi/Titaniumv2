#ifndef CONNECTIONSTATUS_PROTO_H
#define CONNECTIONSTATUS_PROTO_H

#include "string.h"
// #include <cstring>

// #include "stdint.h"
// #include "HAL/memory/MemoryHandlers.h"
#include "Libraries/Json/inc/jsmn.h"
// #include "SystemProcess/NetworkProcess/inc/ConnectionStatusProto.h"
// #include "SystemProcess/ProcessAreasIndex.h"

#include "esp_log.h"

class ConnectionStatusProtobuf {
   public:
    ConnectionStatusProtobuf()  = default;
    ~ConnectionStatusProtobuf() = default;

    uint8_t GetApStatus(void) const {
        return this->_ap_status;
    }
    uint8_t GetStaStatus(void) const {
        return this->_sta_status;
    }

    int16_t GetSerializedSize(void) const {
        return (sizeof(this->_ap_status) + sizeof(this->_sta_status));
    }

    int16_t GetMaxSize(void) {
        return (sizeof(this->_ap_status) + sizeof(this->_sta_status));
    }

    static int16_t GetStaticMaxSize(void) {
        return (sizeof(uint8_t) + sizeof(uint8_t));
    }

    int8_t UpdateApStatus(uint8_t value) {
        this->_ap_status = value;
        return 0;
    }

    int8_t UpdateStaStatus(uint8_t value) {
        this->_sta_status = value;
        return 0;
    }

    int16_t Serialize(char* out_buffer, uint16_t out_buffer_size) const {
        if (out_buffer == nullptr) {
            return 0;
        }

        uint16_t serialized_size = sizeof(this->_ap_status) + sizeof(this->_sta_status);

        if (out_buffer_size < serialized_size) {
            return 0;
        }

        uint16_t offset = 0;

        memcpy(&out_buffer[offset], &this->_ap_status, sizeof(this->_ap_status));
        offset += sizeof(this->_ap_status);
        memcpy(&out_buffer[offset], &this->_sta_status, sizeof(this->_sta_status));

        return serialized_size;
    }

    int8_t DeSerialize(const char* in_buffer, uint16_t in_buffer_size) {
        if (in_buffer == nullptr) {
            return -1;
        }

        uint16_t deserialized_min_size = sizeof(this->_ap_status) + sizeof(this->_sta_status);
        uint16_t deserialized_max_size = sizeof(this->_ap_status) + sizeof(this->_sta_status);

        if (in_buffer_size < deserialized_min_size) {
            return -3;
        }

        if (in_buffer_size > deserialized_max_size) {
            return -3;
        }

        uint16_t offset = 0;
        memcpy(&this->_ap_status, &in_buffer[offset], sizeof(this->_ap_status));
        offset += sizeof(this->_ap_status);
        memcpy(&this->_sta_status, &in_buffer[offset], sizeof(this->_sta_status));

        return 0;
    }

   private:
    uint8_t _ap_status  = 0;
    uint8_t _sta_status = 0;

    // const std::string _JSON_STRING = "{\n\t\"connection_ap_status\": %d,\n\t\"connection_sta_status\": %d\n}";
    // const std::string _CONNECTION_AP_STATUS_TOKEN_VALUE = "connection_ap_status";
    // const std::string _CONNECTION_STA_STATUS_TOKEN_VALUE = "connection_sta_status";
    // const uint8_t _CONNECTION_AP_STATUS_TOKEN  = 1;
    // const uint8_t _CONNECTION_STA_STATUS_TOKEN = 3;
    // const uint8_t _NUM_TOKENS                  = 5;
};

#endif /* CONNECTIONSTATUS_PROTO_H */

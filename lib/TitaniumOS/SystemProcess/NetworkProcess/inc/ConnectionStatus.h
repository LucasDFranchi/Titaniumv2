#ifndef CONNECTION_STATUS_PROTOBUF_H
#define CONNECTION_STATUS_PROTOBUF_H

#include "stdint.h"
#include "string.h"

namespace Errors {
    constexpr int8_t NO_ERROR = 0;
    constexpr int8_t INVALID_BUFFER_PTR = -1;
    constexpr int8_t INVALID_BUFFER_SIZE = -2;
    constexpr int8_t OVERFLOW_BUFFER = -3;
}

class ConnectionStatusProtobuf {
public:
    ConnectionStatusProtobuf() = default;
    ~ConnectionStatusProtobuf() = default;


    uint8_t GetAp_status(void) const {  return this->_ap_status; }
    uint8_t GetSta_status(void) const {  return this->_sta_status; }

    int8_t UpdateAp_status(uint8_t value) {
        this->_ap_status = value;
        return Errors::NO_ERROR;
    }

    int8_t UpdateSta_status(uint8_t value) {
        this->_sta_status = value;
        return Errors::NO_ERROR;
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
            return Errors::INVALID_BUFFER_PTR;
        }

        uint16_t deserialized_min_size = sizeof(this->_ap_status) + sizeof(this->_sta_status);
        uint16_t deserialized_max_size = sizeof(this->_ap_status) + sizeof(this->_sta_status);

        if (in_buffer_size < deserialized_min_size) {
            return Errors::OVERFLOW_BUFFER;
        }

        if (in_buffer_size > deserialized_max_size) {
            return Errors::OVERFLOW_BUFFER;
        }


        uint16_t offset = 0;
        memcpy(&this->_ap_status, &in_buffer[offset], sizeof(this->_ap_status));
        offset += sizeof(this->_ap_status);
        memcpy(&this->_sta_status, &in_buffer[offset], sizeof(this->_sta_status));
    
        return Errors::NO_ERROR;
    }

private:
    uint8_t _ap_status = 0;
    uint8_t _sta_status = 0;
};

#endif /* CONNECTION_STATUS_PROTOBUF_H */
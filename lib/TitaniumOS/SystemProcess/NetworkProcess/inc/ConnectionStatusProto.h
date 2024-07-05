#ifndef CONNECTIONSTATUS_PROTO_H
#define CONNECTIONSTATUS_PROTO_H

#include "stdint.h"
#include "string.h"

class ConnectionStatusProtobuf {
public:
    ConnectionStatusProtobuf() = default;
    ~ConnectionStatusProtobuf() = default;


    uint8_t GetApStatus(void) const {  return this->_ap_status; }
    uint8_t GetStaStatus(void) const {  return this->_sta_status; }

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
    uint8_t _ap_status = 0;
    uint8_t _sta_status = 0;
};

#endif /* CONNECTIONSTATUS_PROTO_H */ 


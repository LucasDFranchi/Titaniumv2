#ifndef WATERLEVEL_PROTO_H
#define WATERLEVEL_PROTO_H

#include "stdint.h"
#include "string.h"

class WaterLevelProtobuf {
public:
    WaterLevelProtobuf() = default;
    ~WaterLevelProtobuf() = default;


    uint32_t GetTimestamp(void) const {  return this->_timestamp; }
    uint32_t GetValue(void) const {  return this->_value; }

    int16_t GetSerializedSize(void) const {
        return (sizeof(this->_timestamp) + sizeof(this->_value));
    }

    int16_t GetMaxSize(void) {
        return (sizeof(this->_timestamp) + sizeof(this->_value));
    }

    static int16_t GetStaticMaxSize(void) {
        return (sizeof(uint32_t) + sizeof(uint32_t));
    }

    int8_t UpdateTimestamp(uint32_t value) {
        this->_timestamp = value;
        return 0;
    }

    int8_t UpdateValue(uint32_t value) {
        this->_value = value;
        return 0;
    }

    int16_t Serialize(char* out_buffer, uint16_t out_buffer_size) const {
        if (out_buffer == nullptr) {
            return 0;
        }

        uint16_t serialized_size = sizeof(this->_timestamp) + sizeof(this->_value);

        if (out_buffer_size < serialized_size) {
            return 0;
        }

        uint16_t offset = 0;

        memcpy(&out_buffer[offset], &this->_timestamp, sizeof(this->_timestamp));
        offset += sizeof(this->_timestamp);
        memcpy(&out_buffer[offset], &this->_value, sizeof(this->_value));
    
        return serialized_size;
    }

    int8_t DeSerialize(const char* in_buffer, uint16_t in_buffer_size) {
        if (in_buffer == nullptr) {
            return -1;
        }

        uint16_t deserialized_min_size = sizeof(this->_timestamp) + sizeof(this->_value);
        uint16_t deserialized_max_size = sizeof(this->_timestamp) + sizeof(this->_value);

        if (in_buffer_size < deserialized_min_size) {
            return -3;
        }

        if (in_buffer_size > deserialized_max_size) {
            return -3;
        }


        uint16_t offset = 0;
        memcpy(&this->_timestamp, &in_buffer[offset], sizeof(this->_timestamp));
        offset += sizeof(this->_timestamp);
        memcpy(&this->_value, &in_buffer[offset], sizeof(this->_value));
    
        return 0;
    }

private:
    uint32_t _timestamp = 0;
    uint32_t _value = 0;
};

#endif /* WATERLEVEL_PROTO_H */ 


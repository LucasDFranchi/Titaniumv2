#ifndef WATER_LEVEL_PROTOBUF_H
#define WATER_LEVEL_PROTOBUF_H

#include "stdint.h"
#include "string.h"

namespace Errors {
    constexpr int8_t NO_ERROR = 0;
    constexpr int8_t INVALID_BUFFER_PTR = -1;
    constexpr int8_t INVALID_BUFFER_SIZE = -2;
    constexpr int8_t OVERFLOW_BUFFER = -3;
}

class WaterLevelProtobuf {
public:
    WaterLevelProtobuf() = default;
    ~WaterLevelProtobuf() = default;


    uint64_t GetTimestamp(void) const {  return this->_timestamp; }
    uint32_t GetValue(void) const {  return this->_value; }

    int8_t UpdateTimestamp(uint64_t value) {
        this->_timestamp = value;
        return Errors::NO_ERROR;
    }

    int8_t UpdateValue(uint32_t value) {
        this->_value = value;
        return Errors::NO_ERROR;
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
            return Errors::INVALID_BUFFER_PTR;
        }

        uint16_t deserialized_min_size = sizeof(this->_timestamp) + sizeof(this->_value);
        uint16_t deserialized_max_size = sizeof(this->_timestamp) + sizeof(this->_value);

        if (in_buffer_size < deserialized_min_size) {
            return Errors::OVERFLOW_BUFFER;
        }

        if (in_buffer_size > deserialized_max_size) {
            return Errors::OVERFLOW_BUFFER;
        }


        uint16_t offset = 0;
        memcpy(&this->_timestamp, &in_buffer[offset], sizeof(this->_timestamp));
        offset += sizeof(this->_timestamp);
        memcpy(&this->_value, &in_buffer[offset], sizeof(this->_value));
    
        return Errors::NO_ERROR;
    }

private:
    uint64_t _timestamp = 0;
    uint32_t _value = 0;
};

#endif /* WATER_LEVEL_PROTOBUF_H */
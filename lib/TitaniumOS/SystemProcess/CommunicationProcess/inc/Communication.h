#ifndef COMMUNICATION_PROTOBUF_H
#define COMMUNICATION_PROTOBUF_H

#include "stdint.h"
#include "string.h"

namespace Errors {
    constexpr int8_t NO_ERROR = 0;
    constexpr int8_t INVALID_BUFFER_PTR = -1;
    constexpr int8_t INVALID_BUFFER_SIZE = -2;
    constexpr int8_t OVERFLOW_BUFFER = -3;
}

class CommunicationProtobuf {
public:
    CommunicationProtobuf() = default;
    ~CommunicationProtobuf() = default;


    uint8_t GetMemory_area(void) const {  return this->_memory_area; }
    uint8_t GetCommand(void) const {  return this->_command; }

    int8_t UpdateMemory_area(uint8_t value) {
        this->_memory_area = value;
        return Errors::NO_ERROR;
    }

    int8_t UpdateCommand(uint8_t value) {
        this->_command = value;
        return Errors::NO_ERROR;
    }

    int16_t Serialize(char* out_buffer, uint16_t out_buffer_size) const {
        if (out_buffer == nullptr) {
            return 0;
        }

        uint16_t serialized_size = sizeof(this->_memory_area) + sizeof(this->_command);

        if (out_buffer_size < serialized_size) {
            return 0;
        }

        uint16_t offset = 0;

        memcpy(&out_buffer[offset], &this->_memory_area, sizeof(this->_memory_area));
        offset += sizeof(this->_memory_area);
        memcpy(&out_buffer[offset], &this->_command, sizeof(this->_command));
    
        return serialized_size;
    }

    int8_t DeSerialize(const char* in_buffer, uint16_t in_buffer_size) {
        if (in_buffer == nullptr) {
            return Errors::INVALID_BUFFER_PTR;
        }

        uint16_t deserialized_min_size = sizeof(this->_memory_area) + sizeof(this->_command);
        uint16_t deserialized_max_size = sizeof(this->_memory_area) + sizeof(this->_command);

        if (in_buffer_size < deserialized_min_size) {
            return Errors::OVERFLOW_BUFFER;
        }

        if (in_buffer_size > deserialized_max_size) {
            return Errors::OVERFLOW_BUFFER;
        }


        uint16_t offset = 0;
        memcpy(&this->_memory_area, &in_buffer[offset], sizeof(this->_memory_area));
        offset += sizeof(this->_memory_area);
        memcpy(&this->_command, &in_buffer[offset], sizeof(this->_command));
    
        return Errors::NO_ERROR;
    }

private:
    uint8_t _memory_area = 0;
    uint8_t _command = 0;
};

#endif /* COMMUNICATION_PROTOBUF_H */
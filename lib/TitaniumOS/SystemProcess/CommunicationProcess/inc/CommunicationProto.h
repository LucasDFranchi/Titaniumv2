#ifndef COMMUNICATION_PROTO_H
#define COMMUNICATION_PROTO_H

#include "stdint.h"
#include "string.h"

class CommunicationProtobuf {
public:
    CommunicationProtobuf() = default;
    ~CommunicationProtobuf() = default;


    uint8_t GetMemoryArea(void) const {  return this->_memory_area; }
    uint8_t GetCommand(void) const {  return this->_command; }

    int16_t GetSerializedSize(void) const {
        return (sizeof(this->_memory_area) + sizeof(this->_command));
    }

    int16_t GetMaxSize(void) {
        return (sizeof(this->_memory_area) + sizeof(this->_command));
    }

    static int16_t GetStaticMaxSize(void) {
        return (sizeof(uint8_t) + sizeof(uint8_t));
    }

    int8_t UpdateMemoryArea(uint8_t value) {
        this->_memory_area = value;
        return 0;
    }

    int8_t UpdateCommand(uint8_t value) {
        this->_command = value;
        return 0;
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
            return -1;
        }

        uint16_t deserialized_min_size = sizeof(this->_memory_area) + sizeof(this->_command);
        uint16_t deserialized_max_size = sizeof(this->_memory_area) + sizeof(this->_command);

        if (in_buffer_size < deserialized_min_size) {
            return -3;
        }

        if (in_buffer_size > deserialized_max_size) {
            return -3;
        }


        uint16_t offset = 0;
        memcpy(&this->_memory_area, &in_buffer[offset], sizeof(this->_memory_area));
        offset += sizeof(this->_memory_area);
        memcpy(&this->_command, &in_buffer[offset], sizeof(this->_command));
    
        return 0;
    }

private:
    uint8_t _memory_area = 0;
    uint8_t _command = 0;
};

#endif /* COMMUNICATION_PROTO_H */ 


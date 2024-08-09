/**
 * @file CommunicationProtobuf.h
 * @brief Auto-generated header file generated.
 */

#ifndef COMMUNICATION_PROTO_H
#define COMMUNICATION_PROTO_H

#include "stdint.h"
#include "string.h"
#include "ArduinoJson.h"
#include "IProtobuf.h"

class CommunicationProtobuf : public IProtobuf {
public:
    CommunicationProtobuf() = default;
    ~CommunicationProtobuf() = default;

    static constexpr uint16_t PAYLOAD_SIZE = 256;

    uint8_t GetCommand(void) const { return this->_command; }
    uint16_t GetAddress(void) const { return this->_address; }
    uint8_t GetMemoryArea(void) const { return this->_memory_area; }
    const char* GetPayload(void) const { return this->_payload; }

    int16_t GetSerializedSize(void) const {
        return (sizeof(this->_command) + sizeof(this->_address) + sizeof(this->_memory_area) + strlen(this->_payload) + 4);
    }

    int16_t GetMaxSize(void) const {
        return (sizeof(this->_command) + sizeof(this->_address) + sizeof(this->_memory_area) + sizeof(this->_payload));
    }

    static int16_t GetStaticMaxSize(void) {
        return (sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint8_t) + PAYLOAD_SIZE);
    }

    int8_t UpdateCommand(uint8_t value) {
        this->_command = value;
        return PROTO_NO_ERROR;
    }

    int8_t UpdateAddress(uint16_t value) {
        this->_address = value;
        return PROTO_NO_ERROR;
    }

    int8_t UpdateMemoryArea(uint8_t value) {
        this->_memory_area = value;
        return PROTO_NO_ERROR;
    }

    int8_t UpdatePayload(const char* value) {
        if (value == nullptr) {
            return PROTO_INVAL_PTR;
        }

        size_t value_length = strlen(value) + 1;

        if ((value_length == 1) || PAYLOAD_SIZE == 0) {
            return PROTO_OVERFLOW;
        }

        if (value_length > PAYLOAD_SIZE) {
            return PROTO_INVAL_SIZE;
        }

        memset(this->_payload, 0, PAYLOAD_SIZE);
        memcpy(this->_payload, value, value_length);

        return PROTO_NO_ERROR;
    }

    int8_t UpdatePayload(const char* value, uint16_t string_size) {
        if (value == nullptr) {
            return PROTO_INVAL_PTR;
        }

        if (PAYLOAD_SIZE == 0) {
            return PROTO_OVERFLOW;
        }

        if (string_size > PAYLOAD_SIZE) {
            return PROTO_INVAL_SIZE;
        }

        memset(this->_payload, 0, PAYLOAD_SIZE);
        memcpy(this->_payload, value, string_size);

        return PROTO_NO_ERROR;
    }

    int16_t Serialize(char* out_buffer, uint16_t out_buffer_size) const {
        uint16_t data_position = 0;
        if (out_buffer == nullptr) {
            return 0;
        }

        uint16_t serialized_size = sizeof(this->_command) + sizeof(this->_address) + sizeof(this->_memory_area) + strlen(this->_payload) + 4;

        if (out_buffer_size < serialized_size) {
            return 0;
        }  

        out_buffer[data_position++] = sizeof(this->_command);
        memcpy(&out_buffer[data_position], &this->_command, sizeof(this->_command));
        data_position += sizeof(this->_command);

        out_buffer[data_position++] = sizeof(this->_address);
        memcpy(&out_buffer[data_position], &this->_address, sizeof(this->_address));
        data_position += sizeof(this->_address);

        out_buffer[data_position++] = sizeof(this->_memory_area);
        memcpy(&out_buffer[data_position], &this->_memory_area, sizeof(this->_memory_area));
        data_position += sizeof(this->_memory_area);

        uint8_t length = strlen(this->_payload);
        out_buffer[data_position++] = length;
        memcpy(&out_buffer[data_position], this->_payload, length);
        data_position += length;

        return serialized_size;
    }

    int8_t DeSerialize(const char* in_buffer, uint16_t in_buffer_size) {
        uint16_t data_position = 0;
        uint8_t size = 0;
                
        if (in_buffer == nullptr) {
            return PROTO_INVAL_PTR;
        }

        uint16_t deserialized_min_size = sizeof(this->_command) + sizeof(this->_address) + sizeof(this->_memory_area) + 1;

        if (in_buffer_size < deserialized_min_size) {
            return PROTO_INVAL_SIZE;
        }

        memset(this->_payload, 0, PAYLOAD_SIZE);

        size = in_buffer[data_position++];
        if (size + data_position > in_buffer_size) return 0;
        memcpy(&this->_command, &in_buffer[data_position], size);
        data_position += size;
    
        size = in_buffer[data_position++];
        if (size + data_position > in_buffer_size) return 0;
        memcpy(&this->_address, &in_buffer[data_position], size);
        data_position += size;
    
        size = in_buffer[data_position++];
        if (size + data_position > in_buffer_size) return 0;
        memcpy(&this->_memory_area, &in_buffer[data_position], size);
        data_position += size;
    
        uint8_t length = in_buffer[data_position++];
        if (length + data_position > in_buffer_size) return 0;
        memcpy(this->_payload, &in_buffer[data_position], length);
        this->_payload[length] = '\0';
        data_position += length;
        return PROTO_NO_ERROR;
    }

    int32_t SerializeJson(char* out_buffer, uint16_t out_buffer_size) {
        uint32_t response_length = 0;

        do {
            if (out_buffer == nullptr) {
                break;
            }

            StaticJsonDocument<512> doc;         
            doc["command"] = this->_command;         
            doc["address"] = this->_address;         
            doc["memory_area"] = this->_memory_area;         
            doc["payload"] = this->_payload;
            response_length = serializeJson(doc, out_buffer, out_buffer_size);
        } while (0);

        return response_length;
    }

    int8_t DeSerializeJson(const char* in_buffer, uint16_t in_buffer_size) {
        auto result = PROTO_NO_ERROR;

        do {
            if (in_buffer == nullptr) {
                result = PROTO_INVAL_PTR;
                break;
            }
            
            StaticJsonDocument<512> doc;
            
            if (deserializeJson(doc, in_buffer)) {
                result = PROTO_INVAL_JSON_PARSE;
                break;
            }
            if (!doc.containsKey("command")) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }
            if (!doc.containsKey("address")) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }
            if (!doc.containsKey("memory_area")) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }
            if (!doc.containsKey("payload")) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }
            if (this->UpdateCommand(doc["command"])) {
                result = PROTO_INVAL_JSON_VALUE;
                break;
            }
            if (this->UpdateAddress(doc["address"])) {
                result = PROTO_INVAL_JSON_VALUE;
                break;
            }
            if (this->UpdateMemoryArea(doc["memory_area"])) {
                result = PROTO_INVAL_JSON_VALUE;
                break;
            }
            if (this->UpdatePayload(doc["payload"])) {
                result = PROTO_INVAL_JSON_VALUE;
                break;
            }

            result = PROTO_NO_ERROR;

        } while(0);

        return result;
    }

private:
    uint8_t _command = 0;
    uint16_t _address = 0;
    uint8_t _memory_area = 0;
    char _payload[256] = {0};
};
#endif /* COMMUNICATION_PROTO_H */

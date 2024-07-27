/**
 * @file CommunicationProtobuf.h
 * @brief Auto-generated header file generated.
 */

#ifndef COMMUNICATION_PROTO_H
#define COMMUNICATION_PROTO_H

#include "stdint.h"
#include "string.h"
#include "Libraries/JSON/jsmn/jsmn.h"
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
        return (sizeof(this->_command) + sizeof(this->_address) + sizeof(this->_memory_area) + (strlen(this->_payload) + 1));
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

        if ((value_length == 0) || PAYLOAD_SIZE == 0) {
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
        if (out_buffer == nullptr) {
            return 0;
        }

        uint16_t serialized_size = sizeof(this->_command) + sizeof(this->_address) + sizeof(this->_memory_area) + (strlen(this->_payload) + 1);

        if (out_buffer_size < serialized_size) {
            return 0;
        }

        uint16_t offset = 0;

        memcpy(&out_buffer[offset], &this->_command, sizeof(this->_command));
        offset += sizeof(this->_command);
        memcpy(&out_buffer[offset], &this->_address, sizeof(this->_address));
        offset += sizeof(this->_address);
        memcpy(&out_buffer[offset], &this->_memory_area, sizeof(this->_memory_area));
        offset += sizeof(this->_memory_area);
        memcpy(&out_buffer[offset], this->_payload, strlen(this->_payload) + 1);

        return serialized_size;
    }

    int8_t DeSerialize(const char* in_buffer, uint16_t in_buffer_size) {
        if (in_buffer == nullptr) {
            return PROTO_INVAL_PTR;
        }

        uint16_t deserialized_min_size = sizeof(this->_command) + sizeof(this->_address) + sizeof(this->_memory_area) + 1;

        if (in_buffer_size < deserialized_min_size) {
            return PROTO_INVAL_SIZE;
        }

        memset(this->_payload, 0, PAYLOAD_SIZE);

        uint16_t offset = 0;
        memcpy(&this->_command, &in_buffer[offset], sizeof(this->_command));
        offset += sizeof(this->_command);
        memcpy(&this->_address, &in_buffer[offset], sizeof(this->_address));
        offset += sizeof(this->_address);
        memcpy(&this->_memory_area, &in_buffer[offset], sizeof(this->_memory_area));
        offset += sizeof(this->_memory_area);
        memcpy(this->_payload, &in_buffer[offset], strlen(&in_buffer[offset]) + 1);

        return PROTO_NO_ERROR;
    }
    int32_t SerializeJson(char* out_buffer, uint16_t out_buffer_size) {
        uint32_t response_length = 0;

        do {
            if (out_buffer == nullptr) {
                break;
            }

            uint16_t serialized_size = sizeof(this->_command) + sizeof(this->_address) + sizeof(this->_memory_area) + (strlen(this->_payload) + 1);

            if (out_buffer_size < serialized_size) {
                return 0;
            }

            response_length = snprintf(out_buffer, out_buffer_size,
                                       this->_json_string,
                                       this->_command,
                                       this->_address,
                                       this->_memory_area,
                                       this->_payload);
        } while (0);

        return response_length;
    }

    int8_t DeSerializeJson(const char* in_buffer, uint16_t in_buffer_size) {
        auto result = PROTO_NO_ERROR;
        jsmn_parser parser;
        jsmntok_t tokens[this->_NUM_TOKENS];

        jsmn_init(&parser);

        do {
            if (in_buffer == nullptr) {
                result = PROTO_INVAL_PTR;
                break;
            }

            auto num_tokens = jsmn_parse(&parser, in_buffer, strlen(in_buffer), tokens, this->_NUM_TOKENS);

            if (num_tokens != this->_NUM_TOKENS) {
                result = PROTO_INVAL_NUM_TOKEN;
                break;
            }

            jsmntok_t key{};
            jsmntok_t value{};
            uint16_t token_length = 0;

            key   = tokens[this->_COMMAND_TOKEN_ID];
            value = tokens[this->_COMMAND_TOKEN_ID + 1];
            token_length = key.end - key.start;

            if (strncmp(in_buffer + key.start, this->_COMMAND_TOKEN_NAME, token_length) != 0) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }

            this->UpdateCommand(atoi(in_buffer + value.start));

            key   = tokens[this->_ADDRESS_TOKEN_ID];
            value = tokens[this->_ADDRESS_TOKEN_ID + 1];
            token_length = key.end - key.start;

            if (strncmp(in_buffer + key.start, this->_ADDRESS_TOKEN_NAME, token_length) != 0) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }

            this->UpdateAddress(atoi(in_buffer + value.start));

            key   = tokens[this->_MEMORY_AREA_TOKEN_ID];
            value = tokens[this->_MEMORY_AREA_TOKEN_ID + 1];
            token_length = key.end - key.start;

            if (strncmp(in_buffer + key.start, this->_MEMORY_AREA_TOKEN_NAME, token_length) != 0) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }

            this->UpdateMemoryArea(atoi(in_buffer + value.start));

            key   = tokens[this->_PAYLOAD_TOKEN_ID];
            value = tokens[this->_PAYLOAD_TOKEN_ID + 1];
            token_length = key.end - key.start;

            if (strncmp(in_buffer + key.start, this->_PAYLOAD_TOKEN_NAME, token_length) != 0) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }

            this->UpdatePayload(in_buffer + value.start, value.end - value.start);

            result = PROTO_NO_ERROR;

        } while(0);

        return result;
    }

private:
    uint8_t _command = 0;
    uint16_t _address = 0;
    uint8_t _memory_area = 0;
    char _payload[256] = {0};
    const char* _json_string = R"({
    "command": %u,
    "address": %u,
    "memory_area": %u,
    "payload": "%s"
})";  
    const char* _COMMAND_TOKEN_NAME = "command";
    const uint8_t _COMMAND_TOKEN_ID = 1;  
    const char* _ADDRESS_TOKEN_NAME = "address";
    const uint8_t _ADDRESS_TOKEN_ID = 3;  
    const char* _MEMORY_AREA_TOKEN_NAME = "memory_area";
    const uint8_t _MEMORY_AREA_TOKEN_ID = 5;  
    const char* _PAYLOAD_TOKEN_NAME = "payload";
    const uint8_t _PAYLOAD_TOKEN_ID = 7;
    const uint8_t _NUM_TOKENS  = 9;
};
#endif /* COMMUNICATION_PROTO_H */

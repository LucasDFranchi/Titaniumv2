/**
 * @file CommunicationProtobuf.h
 * @brief Auto-generated header file generated.
 */

#ifndef COMMUNICATION_PROTO_H
#define COMMUNICATION_PROTO_H

#include "stdint.h"
#include "string.h"
#include "jsmn.h"

#ifndef PROTOBUFS_ERRORS_H
#define PROTOBUFS_ERRORS_H

enum protobufs_errors {
  PROTO_NO_ERROR   = 0,
  PROTO_INVAL_PTR  = -1,
  PROTO_OVERFLOW   = -2,
  PROTO_INVAL_SIZE = -3,
  PROTO_INVAL_NUM_TOKEN = -4,
  PROTO_INVAL_JSON_KEY = -5,
};

#endif // PROTOBUFS_ERRORS_H

class CommunicationProtobuf {
public:
    CommunicationProtobuf() = default;
    ~CommunicationProtobuf() = default;


    uint8_t GetMemoryArea(void) const { return this->_memory_area; }
    uint8_t GetCommand(void) const { return this->_command; }

    int16_t GetSerializedSize(void) const {
        return (sizeof(this->_memory_area) + sizeof(this->_command));
    }

    int16_t GetMaxSize(void) const {
        return (sizeof(this->_memory_area) + sizeof(this->_command));
    }

    static int16_t GetStaticMaxSize(void) {
        return (sizeof(uint8_t) + sizeof(uint8_t));
    }

    int8_t UpdateMemoryArea(uint8_t value) {
        this->_memory_area = value;
        return PROTO_NO_ERROR;
    }

    int8_t UpdateCommand(uint8_t value) {
        this->_command = value;
        return PROTO_NO_ERROR;
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
            return PROTO_INVAL_PTR;
        }

        uint16_t deserialized_min_size = sizeof(this->_memory_area) + sizeof(this->_command) + 0;
        uint16_t deserialized_max_size = sizeof(this->_memory_area) + sizeof(this->_command);

        if ((in_buffer_size < deserialized_min_size) || (in_buffer_size > deserialized_max_size)) {
            return PROTO_INVAL_SIZE;
        }


        uint16_t offset = 0;
        memcpy(&this->_memory_area, &in_buffer[offset], sizeof(this->_memory_area));
        offset += sizeof(this->_memory_area);
        memcpy(&this->_command, &in_buffer[offset], sizeof(this->_command));

        return PROTO_NO_ERROR;
    }
    int32_t SerializeJson(char* out_buffer, uint16_t out_buffer_size) {
        uint32_t response_length = 0;

        do {
            if (out_buffer == nullptr) {
                break;
            }

            uint16_t serialized_size = sizeof(this->_memory_area) + sizeof(this->_command);

            if (out_buffer_size < serialized_size) {
                return 0;
            }

            response_length = snprintf(out_buffer, out_buffer_size,
                                       this->_json_string,
                                       this->_memory_area,
                                       this->_command);
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

            key   = tokens[this->_MEMORY_AREA_TOKEN_ID];
            value = tokens[this->_MEMORY_AREA_TOKEN_ID + 1];
            token_length = key.end - key.start;

            if (strncmp(in_buffer + key.start, this->_MEMORY_AREA_TOKEN_NAME, token_length) != 0) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }

            this->UpdateMemoryArea(atoi(in_buffer + value.start));

            key   = tokens[this->_COMMAND_TOKEN_ID];
            value = tokens[this->_COMMAND_TOKEN_ID + 1];
            token_length = key.end - key.start;

            if (strncmp(in_buffer + key.start, this->_COMMAND_TOKEN_NAME, token_length) != 0) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }

            this->UpdateCommand(atoi(in_buffer + value.start));

            result = PROTO_NO_ERROR;

        } while(0);

        return result;
    }

private:
    uint8_t _memory_area = 0;
    uint8_t _command = 0;
    const char* _json_string = R"({
    "memory_area": %u,
    "command": %u
})";  
    const char* _MEMORY_AREA_TOKEN_NAME = "memory_area";
    const uint8_t _MEMORY_AREA_TOKEN_ID = 1;  
    const char* _COMMAND_TOKEN_NAME = "command";
    const uint8_t _COMMAND_TOKEN_ID = 3;
    const uint8_t _NUM_TOKENS  = 5;
};
#endif /* COMMUNICATION_PROTO_H */

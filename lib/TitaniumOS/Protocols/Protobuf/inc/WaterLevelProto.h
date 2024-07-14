/**
 * @file WaterLevelProtobuf.h
 * @brief Auto-generated header file generated.
 */

#ifndef WATERLEVEL_PROTO_H
#define WATERLEVEL_PROTO_H

#include "stdint.h"
#include "string.h"
#include "jsmn.h"
#include "IProtobuf.h"

class WaterLevelProtobuf : public IProtobuf {
public:
    WaterLevelProtobuf() = default;
    ~WaterLevelProtobuf() = default;


    uint64_t GetTimestamp(void) const { return this->_timestamp; }
    uint32_t GetValue(void) const { return this->_value; }

    int16_t GetSerializedSize(void) const {
        return (sizeof(this->_timestamp) + sizeof(this->_value));
    }

    int16_t GetMaxSize(void) const {
        return (sizeof(this->_timestamp) + sizeof(this->_value));
    }

    static int16_t GetStaticMaxSize(void) {
        return (sizeof(uint64_t) + sizeof(uint32_t));
    }

    int8_t UpdateTimestamp(uint64_t value) {
        this->_timestamp = value;
        return PROTO_NO_ERROR;
    }

    int8_t UpdateValue(uint32_t value) {
        this->_value = value;
        return PROTO_NO_ERROR;
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
            return PROTO_INVAL_PTR;
        }

        uint16_t deserialized_min_size = sizeof(this->_timestamp) + sizeof(this->_value) + 0;
        uint16_t deserialized_max_size = sizeof(this->_timestamp) + sizeof(this->_value);

        if ((in_buffer_size < deserialized_min_size) || (in_buffer_size > deserialized_max_size)) {
            return PROTO_INVAL_SIZE;
        }


        uint16_t offset = 0;
        memcpy(&this->_timestamp, &in_buffer[offset], sizeof(this->_timestamp));
        offset += sizeof(this->_timestamp);
        memcpy(&this->_value, &in_buffer[offset], sizeof(this->_value));

        return PROTO_NO_ERROR;
    }
    int32_t SerializeJson(char* out_buffer, uint16_t out_buffer_size) {
        uint32_t response_length = 0;

        do {
            if (out_buffer == nullptr) {
                break;
            }

            uint16_t serialized_size = sizeof(this->_timestamp) + sizeof(this->_value);

            if (out_buffer_size < serialized_size) {
                return 0;
            }

            response_length = snprintf(out_buffer, out_buffer_size,
                                       this->_json_string,
                                       this->_timestamp,
                                       this->_value);
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

            key   = tokens[this->_TIMESTAMP_TOKEN_ID];
            value = tokens[this->_TIMESTAMP_TOKEN_ID + 1];
            token_length = key.end - key.start;

            if (strncmp(in_buffer + key.start, this->_TIMESTAMP_TOKEN_NAME, token_length) != 0) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }

            this->UpdateTimestamp(atoi(in_buffer + value.start));

            key   = tokens[this->_VALUE_TOKEN_ID];
            value = tokens[this->_VALUE_TOKEN_ID + 1];
            token_length = key.end - key.start;

            if (strncmp(in_buffer + key.start, this->_VALUE_TOKEN_NAME, token_length) != 0) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }

            this->UpdateValue(atoi(in_buffer + value.start));

            result = PROTO_NO_ERROR;

        } while(0);

        return result;
    }

private:
    uint64_t _timestamp = 0;
    uint32_t _value = 0;
    const char* _json_string = R"({
    "timestamp": %llu,
    "value": %u
})";  
    const char* _TIMESTAMP_TOKEN_NAME = "timestamp";
    const uint8_t _TIMESTAMP_TOKEN_ID = 1;  
    const char* _VALUE_TOKEN_NAME = "value";
    const uint8_t _VALUE_TOKEN_ID = 3;
    const uint8_t _NUM_TOKENS  = 5;
};
#endif /* WATERLEVEL_PROTO_H */

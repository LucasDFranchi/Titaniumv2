/**
 * @file ConnectionStatusProtobuf.h
 * @brief Auto-generated header file generated.
 */

#ifndef CONNECTIONSTATUS_PROTO_H
#define CONNECTIONSTATUS_PROTO_H

#include "stdint.h"
#include "string.h"
#include "jsmn.h"
#include "IProtobuf.h"

class ConnectionStatusProtobuf : public IProtobuf {
public:
    ConnectionStatusProtobuf() = default;
    ~ConnectionStatusProtobuf() = default;


    uint8_t GetApStatus(void) const { return this->_ap_status; }
    uint8_t GetStaStatus(void) const { return this->_sta_status; }

    int16_t GetSerializedSize(void) const {
        return (sizeof(this->_ap_status) + sizeof(this->_sta_status));
    }

    int16_t GetMaxSize(void) const {
        return (sizeof(this->_ap_status) + sizeof(this->_sta_status));
    }

    static int16_t GetStaticMaxSize(void) {
        return (sizeof(uint8_t) + sizeof(uint8_t));
    }

    int8_t UpdateApStatus(uint8_t value) {
        this->_ap_status = value;
        return PROTO_NO_ERROR;
    }

    int8_t UpdateStaStatus(uint8_t value) {
        this->_sta_status = value;
        return PROTO_NO_ERROR;
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
            return PROTO_INVAL_PTR;
        }

        uint16_t deserialized_min_size = sizeof(this->_ap_status) + sizeof(this->_sta_status) + 0;
        uint16_t deserialized_max_size = sizeof(this->_ap_status) + sizeof(this->_sta_status);

        if ((in_buffer_size < deserialized_min_size) || (in_buffer_size > deserialized_max_size)) {
            return PROTO_INVAL_SIZE;
        }


        uint16_t offset = 0;
        memcpy(&this->_ap_status, &in_buffer[offset], sizeof(this->_ap_status));
        offset += sizeof(this->_ap_status);
        memcpy(&this->_sta_status, &in_buffer[offset], sizeof(this->_sta_status));

        return PROTO_NO_ERROR;
    }
    int32_t SerializeJson(char* out_buffer, uint16_t out_buffer_size) {
        uint32_t response_length = 0;

        do {
            if (out_buffer == nullptr) {
                break;
            }

            uint16_t serialized_size = sizeof(this->_ap_status) + sizeof(this->_sta_status);

            if (out_buffer_size < serialized_size) {
                return 0;
            }

            response_length = snprintf(out_buffer, out_buffer_size,
                                       this->_json_string,
                                       this->_ap_status,
                                       this->_sta_status);
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

            key   = tokens[this->_AP_STATUS_TOKEN_ID];
            value = tokens[this->_AP_STATUS_TOKEN_ID + 1];
            token_length = key.end - key.start;

            if (strncmp(in_buffer + key.start, this->_AP_STATUS_TOKEN_NAME, token_length) != 0) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }

            this->UpdateApStatus(atoi(in_buffer + value.start));

            key   = tokens[this->_STA_STATUS_TOKEN_ID];
            value = tokens[this->_STA_STATUS_TOKEN_ID + 1];
            token_length = key.end - key.start;

            if (strncmp(in_buffer + key.start, this->_STA_STATUS_TOKEN_NAME, token_length) != 0) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }

            this->UpdateStaStatus(atoi(in_buffer + value.start));

            result = PROTO_NO_ERROR;

        } while(0);

        return result;
    }

private:
    uint8_t _ap_status = 0;
    uint8_t _sta_status = 0;
    const char* _json_string = R"({
    "ap_status": %u,
    "sta_status": %u
})";  
    const char* _AP_STATUS_TOKEN_NAME = "ap_status";
    const uint8_t _AP_STATUS_TOKEN_ID = 1;  
    const char* _STA_STATUS_TOKEN_NAME = "sta_status";
    const uint8_t _STA_STATUS_TOKEN_ID = 3;
    const uint8_t _NUM_TOKENS  = 5;
};
#endif /* CONNECTIONSTATUS_PROTO_H */

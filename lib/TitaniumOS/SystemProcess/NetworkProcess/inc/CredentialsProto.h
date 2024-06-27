/**
 * @file CredentialsProtobuf.h
 * @brief Auto-generated header file generated.
 */

#ifndef CREDENTIALS_PROTO_H
#define CREDENTIALS_PROTO_H

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

class CredentialsProtobuf {
public:
    CredentialsProtobuf() = default;
    ~CredentialsProtobuf() = default;

    static constexpr uint16_t SSID_SIZE = 32;
    static constexpr uint16_t PASSWORD_SIZE = 64;

    const char* GetSsid(void) const { return this->_ssid; }
    const char* GetPassword(void) const { return this->_password; }

    int16_t GetSerializedSize(void) const {
        return ((strlen(this->_ssid) + 1) + (strlen(this->_password) + 1));
    }

    int16_t GetMaxSize(void) const {
        return (sizeof(this->_ssid) + sizeof(this->_password));
    }

    static int16_t GetStaticMaxSize(void) {
        return (SSID_SIZE + PASSWORD_SIZE);
    }

    int8_t UpdateSsid(const char* value) {
        if (value == nullptr) {
            return PROTO_INVAL_PTR;
        }

        size_t value_length = strlen(value) + 1;

        if ((value_length == 0) || SSID_SIZE == 0) {
            return PROTO_OVERFLOW;
        }

        if (value_length > SSID_SIZE) {
            return PROTO_INVAL_SIZE;
        }

        memset(this->_ssid, 0, SSID_SIZE);
        memcpy(this->_ssid, value, value_length);

        return PROTO_NO_ERROR;
    }

    int8_t UpdateSsid(const char* value, uint16_t string_size) {
        if (value == nullptr) {
            return PROTO_INVAL_PTR;
        }

        if (SSID_SIZE == 0) {
            return PROTO_OVERFLOW;
        }

        if (string_size > SSID_SIZE) {
            return PROTO_INVAL_SIZE;
        }

        memset(this->_ssid, 0, SSID_SIZE);
        memcpy(this->_ssid, value, string_size);

        return PROTO_NO_ERROR;
    }

    int8_t UpdatePassword(const char* value) {
        if (value == nullptr) {
            return PROTO_INVAL_PTR;
        }

        size_t value_length = strlen(value) + 1;

        if ((value_length == 0) || PASSWORD_SIZE == 0) {
            return PROTO_OVERFLOW;
        }

        if (value_length > PASSWORD_SIZE) {
            return PROTO_INVAL_SIZE;
        }

        memset(this->_password, 0, PASSWORD_SIZE);
        memcpy(this->_password, value, value_length);

        return PROTO_NO_ERROR;
    }

    int8_t UpdatePassword(const char* value, uint16_t string_size) {
        if (value == nullptr) {
            return PROTO_INVAL_PTR;
        }

        if (PASSWORD_SIZE == 0) {
            return PROTO_OVERFLOW;
        }

        if (string_size > PASSWORD_SIZE) {
            return PROTO_INVAL_SIZE;
        }

        memset(this->_password, 0, PASSWORD_SIZE);
        memcpy(this->_password, value, string_size);

        return PROTO_NO_ERROR;
    }

    int16_t Serialize(char* out_buffer, uint16_t out_buffer_size) const {
        if (out_buffer == nullptr) {
            return 0;
        }

        uint16_t serialized_size = (strlen(this->_ssid) + 1) + (strlen(this->_password) + 1);

        if (out_buffer_size < serialized_size) {
            return 0;
        }

        uint16_t offset = 0;

        memcpy(&out_buffer[offset], this->_ssid, strlen(this->_ssid) + 1);
        offset += strlen(this->_ssid) + 1;
        memcpy(&out_buffer[offset], this->_password, strlen(this->_password) + 1);

        return serialized_size;
    }

    int8_t DeSerialize(const char* in_buffer, uint16_t in_buffer_size) {
        if (in_buffer == nullptr) {
            return PROTO_INVAL_PTR;
        }

        uint16_t deserialized_min_size =  + 2;
        uint16_t deserialized_max_size = sizeof(this->_ssid) + sizeof(this->_password);

        if ((in_buffer_size < deserialized_min_size) || (in_buffer_size > deserialized_max_size)) {
            return PROTO_INVAL_SIZE;
        }

        memset(this->_ssid, 0, SSID_SIZE);
        memset(this->_password, 0, PASSWORD_SIZE);

        uint16_t offset = 0;
        memcpy(this->_ssid, &in_buffer[offset], strlen(&in_buffer[offset]) + 1);
        offset += strlen(&in_buffer[offset]) + 1;
        memcpy(this->_password, &in_buffer[offset], strlen(&in_buffer[offset]) + 1);

        return PROTO_NO_ERROR;
    }
    int32_t SerializeJson(char* out_buffer, uint16_t out_buffer_size) {
        uint32_t response_length = 0;

        do {
            if (out_buffer == nullptr) {
                break;
            }

            uint16_t serialized_size = (strlen(this->_ssid) + 1) + (strlen(this->_password) + 1);

            if (out_buffer_size < serialized_size) {
                return 0;
            }

            response_length = snprintf(out_buffer, out_buffer_size,
                                       this->_json_string,
                                       this->_ssid,
                                       this->_password);
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

            key   = tokens[this->_SSID_TOKEN_ID];
            value = tokens[this->_SSID_TOKEN_ID + 1];
            token_length = key.end - key.start;

            if (strncmp(in_buffer + key.start, this->_SSID_TOKEN_NAME, token_length) != 0) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }

            this->UpdateSsid(in_buffer + value.start, value.end - value.start);

            key   = tokens[this->_PASSWORD_TOKEN_ID];
            value = tokens[this->_PASSWORD_TOKEN_ID + 1];
            token_length = key.end - key.start;

            if (strncmp(in_buffer + key.start, this->_PASSWORD_TOKEN_NAME, token_length) != 0) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }

            this->UpdatePassword(in_buffer + value.start, value.end - value.start);

            result = PROTO_NO_ERROR;

        } while(0);

        return result;
    }

private:
    char _ssid[32] = {0};
    char _password[64] = {0};
    const char* _json_string = R"({
    "ssid": "%s",
    "password": "%s"
})";  
    const char* _SSID_TOKEN_NAME = "ssid";
    const uint8_t _SSID_TOKEN_ID = 1;  
    const char* _PASSWORD_TOKEN_NAME = "password";
    const uint8_t _PASSWORD_TOKEN_ID = 3;
    const uint8_t _NUM_TOKENS  = 5;
};
#endif /* CREDENTIALS_PROTO_H */

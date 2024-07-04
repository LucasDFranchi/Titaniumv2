#ifndef CREDENTIALS_PROTOBUF_H
#define CREDENTIALS_PROTOBUF_H


#include "stdint.h"
#include "string.h"

namespace Errors {
    constexpr int8_t NO_ERROR = 0;
    constexpr int8_t INVALID_BUFFER_PTR = -1;
    constexpr int8_t INVALID_BUFFER_SIZE = -2;
    constexpr int8_t OVERFLOW_BUFFER = -3;
}

class CredentialsProtobuf {
public:
    CredentialsProtobuf() = default;
    ~CredentialsProtobuf() = default;

    static constexpr uint16_t SSID_SIZE = 32;
    static constexpr uint16_t PASSWORD_SIZE = 64;

    const char* GetSsid(void) const {  return this->_ssid; }
    const char* GetPassword(void) const {  return this->_password; }

    int8_t UpdateSsid(char* value) {
        if (value == nullptr || this->_ssid == nullptr) {
            return Errors::INVALID_BUFFER_PTR;
        }

        size_t value_length = strlen(value) + 1;

        if ((value_length == 0) || SSID_SIZE == 0) {
            return Errors::INVALID_BUFFER_SIZE;
        }

        if (value_length > SSID_SIZE) {
            return Errors::OVERFLOW_BUFFER;
        }

        memset(this->_ssid, 0, SSID_SIZE);
        memcpy(this->_ssid, value, value_length);

        return Errors::NO_ERROR;
    }

    int8_t UpdatePassword(char* value) {
        if (value == nullptr || this->_password == nullptr) {
            return Errors::INVALID_BUFFER_PTR;
        }

        size_t value_length = strlen(value) + 1;

        if ((value_length == 0) || PASSWORD_SIZE == 0) {
            return Errors::INVALID_BUFFER_SIZE;
        }

        if (value_length > PASSWORD_SIZE) {
            return Errors::OVERFLOW_BUFFER;
        }

        memset(this->_password, 0, PASSWORD_SIZE);
        memcpy(this->_password, value, value_length);

        return Errors::NO_ERROR;
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
            return Errors::INVALID_BUFFER_PTR;
        }

        uint16_t deserialized_min_size = 1 + 1;
        uint16_t deserialized_max_size = sizeof(this->_ssid) + sizeof(this->_password);

        if (in_buffer_size < deserialized_min_size) {
            return Errors::OVERFLOW_BUFFER;
        }

        if (in_buffer_size > deserialized_max_size) {
            return Errors::OVERFLOW_BUFFER;
        }

        memset(this->_ssid, 0, SSID_SIZE);
        memset(this->_password, 0, PASSWORD_SIZE);

        uint16_t offset = 0;
        memcpy(this->_ssid, &in_buffer[offset], strlen(&in_buffer[offset]) + 1);
        offset += strlen(&in_buffer[offset]) + 1;
        memcpy(this->_password, &in_buffer[offset], strlen(&in_buffer[offset]) + 1);
    
        return Errors::NO_ERROR;
    }

private:
    char _ssid[32] = {0};
    char _password[64] = {0};
};

#endif /* CREDENTIALS_PROTOBUF_H */
/**
 * @file CredentialsProtobuf.h
 * @brief Auto-generated header file generated.
 */

#ifndef CREDENTIALS_PROTO_H
#define CREDENTIALS_PROTO_H

#include "stdint.h"
#include "string.h"
#include "Libraries/JSON/ArduinoJson/ArduinoJson.h"
#include "IProtobuf.h"

class CredentialsProtobuf : public IProtobuf {
public:
    CredentialsProtobuf() = default;
    ~CredentialsProtobuf() = default;

    static constexpr uint16_t SSID_SIZE = 32;
    static constexpr uint16_t PASSWORD_SIZE = 64;

    const char* GetSsid(void) const { return this->_ssid; }
    const char* GetPassword(void) const { return this->_password; }

    int16_t GetSerializedSize(void) const {
        return (strlen(this->_ssid) + strlen(this->_password) + 2);
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

        if ((value_length == 1) || SSID_SIZE == 0) {
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

        if ((value_length == 1) || PASSWORD_SIZE == 0) {
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
        uint16_t data_position = 0;
        if (out_buffer == nullptr) {
            return 0;
        }

        uint16_t serialized_size = strlen(this->_ssid) + strlen(this->_password) + 2;

        if (out_buffer_size < serialized_size) {
            return 0;
        }  

        uint8_t length = strlen(this->_ssid);
        out_buffer[data_position++] = length;
        memcpy(&out_buffer[data_position], this->_ssid, length);
        data_position += length;

        length = strlen(this->_password);
        out_buffer[data_position++] = length;
        memcpy(&out_buffer[data_position], this->_password, length);
        data_position += length;

        return serialized_size;
    }

    int8_t DeSerialize(const char* in_buffer, uint16_t in_buffer_size) {
        uint16_t data_position = 0;
        // uint8_t size = 0;
                
        if (in_buffer == nullptr) {
            return PROTO_INVAL_PTR;
        }

        uint16_t deserialized_min_size =  + 2;

        if (in_buffer_size < deserialized_min_size) {
            return PROTO_INVAL_SIZE;
        }

        memset(this->_ssid, 0, SSID_SIZE);
        memset(this->_password, 0, PASSWORD_SIZE);

        uint8_t length = in_buffer[data_position++];
        if (length + data_position > in_buffer_size) return 0;
        memcpy(this->_ssid, &in_buffer[data_position], length);
        this->_ssid[length] = '\0';
        data_position += length;
        length = in_buffer[data_position++];
        if (length + data_position > in_buffer_size) return 0;
        memcpy(this->_password, &in_buffer[data_position], length);
        this->_password[length] = '\0';
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
            doc["ssid"] = this->_ssid;         
            doc["password"] = this->_password;
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
            if (!doc.containsKey("ssid")) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }
            if (!doc.containsKey("password")) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }
            if (this->UpdateSsid(doc["ssid"])) {
                result = PROTO_INVAL_JSON_VALUE;
                break;
            }
            if (this->UpdatePassword(doc["password"])) {
                result = PROTO_INVAL_JSON_VALUE;
                break;
            }

            result = PROTO_NO_ERROR;

        } while(0);

        return result;
    }

private:
    char _ssid[32] = {0};
    char _password[64] = {0};
};
#endif /* CREDENTIALS_PROTO_H */

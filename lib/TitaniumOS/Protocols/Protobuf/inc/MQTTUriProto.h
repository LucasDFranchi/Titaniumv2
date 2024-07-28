/**
 * @file MQTTUriProtobuf.h
 * @brief Auto-generated header file generated.
 */

#ifndef MQTTURI_PROTO_H
#define MQTTURI_PROTO_H

#include "stdint.h"
#include "string.h"
#include "Libraries/JSON/jsmn/jsmn.h"
#include "IProtobuf.h"

class MQTTUriProtobuf : public IProtobuf {
public:
    MQTTUriProtobuf() = default;
    ~MQTTUriProtobuf() = default;

    static constexpr uint16_t MQTT_URI_SIZE = 256;

    const char* GetMqttUri(void) const { return this->_mqtt_uri; }

    int16_t GetSerializedSize(void) const {
        return ((strlen(this->_mqtt_uri) + 1));
    }

    int16_t GetMaxSize(void) const {
        return (sizeof(this->_mqtt_uri));
    }

    static int16_t GetStaticMaxSize(void) {
        return (MQTT_URI_SIZE);
    }

    int8_t UpdateMqttUri(const char* value) {
        if (value == nullptr) {
            return PROTO_INVAL_PTR;
        }

        size_t value_length = strlen(value) + 1;

        if ((value_length == 0) || MQTT_URI_SIZE == 0) {
            return PROTO_OVERFLOW;
        }

        if (value_length > MQTT_URI_SIZE) {
            return PROTO_INVAL_SIZE;
        }

        memset(this->_mqtt_uri, 0, MQTT_URI_SIZE);
        memcpy(this->_mqtt_uri, value, value_length);

        return PROTO_NO_ERROR;
    }

    int8_t UpdateMqttUri(const char* value, uint16_t string_size) {
        if (value == nullptr) {
            return PROTO_INVAL_PTR;
        }

        if (MQTT_URI_SIZE == 0) {
            return PROTO_OVERFLOW;
        }

        if (string_size > MQTT_URI_SIZE) {
            return PROTO_INVAL_SIZE;
        }

        memset(this->_mqtt_uri, 0, MQTT_URI_SIZE);
        memcpy(this->_mqtt_uri, value, string_size);

        return PROTO_NO_ERROR;
    }

    int16_t Serialize(char* out_buffer, uint16_t out_buffer_size) const {
        if (out_buffer == nullptr) {
            return 0;
        }

        uint16_t serialized_size = (strlen(this->_mqtt_uri) + 1) + 1;

        if (out_buffer_size < serialized_size) {
            return 0;
        }

        auto result = snprintf(out_buffer, out_buffer_size,"%s",this->_mqtt_uri);

        if (result > out_buffer_size) {
            serialized_size = 0;
        }
        else if (result == 0) {
            serialized_size = 0;
        }

        return serialized_size;
    }

    int8_t DeSerialize(const char* in_buffer, uint16_t in_buffer_size) {
        if (in_buffer == nullptr) {
            return PROTO_INVAL_PTR;
        }

        uint16_t deserialized_min_size =  + 1;
        
        if (in_buffer_size < deserialized_min_size) {
            return PROTO_INVAL_SIZE;
        }
        
        memset(this->_mqtt_uri, 0, MQTT_URI_SIZE);
        
        uint16_t offset = 0;
        char* token = strtok(const_cast<char*>(in_buffer), "|");
        strncpy(this->_mqtt_uri, token, 256);

        return PROTO_NO_ERROR;
    }
    int32_t SerializeJson(char* out_buffer, uint16_t out_buffer_size) {
        uint32_t response_length = 0;

        do {
            if (out_buffer == nullptr) {
                break;
            }

            uint16_t serialized_size = (strlen(this->_mqtt_uri) + 1);

            if (out_buffer_size < serialized_size) {
                return 0;
            }

            response_length = snprintf(out_buffer, out_buffer_size,
                                       this->_json_string,
                                       this->_mqtt_uri);
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

            key   = tokens[this->_MQTT_URI_TOKEN_ID];
            value = tokens[this->_MQTT_URI_TOKEN_ID + 1];
            token_length = key.end - key.start;

            if (strncmp(in_buffer + key.start, this->_MQTT_URI_TOKEN_NAME, token_length) != 0) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }

            this->UpdateMqttUri(in_buffer + value.start, value.end - value.start);

            result = PROTO_NO_ERROR;

        } while(0);

        return result;
    }

private:
    char _mqtt_uri[256] = {0};
    const char* _json_string = R"({
    "mqtt_uri": "%s"
})";  
    const char* _MQTT_URI_TOKEN_NAME = "mqtt_uri";
    const uint8_t _MQTT_URI_TOKEN_ID = 1;
    const uint8_t _NUM_TOKENS  = 3;
};
#endif /* MQTTURI_PROTO_H */
/**
 * @file MQTTUriProtobuf.h
 * @brief Auto-generated header file generated.
 */

#ifndef MQTTURI_PROTO_H
#define MQTTURI_PROTO_H

#include "stdint.h"
#include "string.h"
#include "Libraries/JSON/ArduinoJson/ArduinoJson.h"
#include "IProtobuf.h"

class MQTTUriProtobuf : public IProtobuf {
public:
    MQTTUriProtobuf() = default;
    ~MQTTUriProtobuf() = default;

    static constexpr uint16_t MQTT_URI_SIZE = 256;

    const char* GetMqttUri(void) const { return this->_mqtt_uri; }

    int16_t GetSerializedSize(void) const {
        return (strlen(this->_mqtt_uri) + 1);
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

        if ((value_length == 1) || MQTT_URI_SIZE == 0) {
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
        uint16_t data_position = 0;
        if (out_buffer == nullptr) {
            return 0;
        }

        uint16_t serialized_size = strlen(this->_mqtt_uri) + 1;

        if (out_buffer_size < serialized_size) {
            return 0;
        }  

        uint8_t length = strlen(this->_mqtt_uri);
        out_buffer[data_position++] = length;
        memcpy(&out_buffer[data_position], this->_mqtt_uri, length);
        data_position += length;

        return serialized_size;
    }

    int8_t DeSerialize(const char* in_buffer, uint16_t in_buffer_size) {
        uint16_t data_position = 0;
        // uint8_t size = 0;
                
        if (in_buffer == nullptr) {
            return PROTO_INVAL_PTR;
        }

        uint16_t deserialized_min_size =  + 1;

        if (in_buffer_size < deserialized_min_size) {
            return PROTO_INVAL_SIZE;
        }

        memset(this->_mqtt_uri, 0, MQTT_URI_SIZE);

        uint8_t length = in_buffer[data_position++];
        if (length + data_position > in_buffer_size) return 0;
        memcpy(this->_mqtt_uri, &in_buffer[data_position], length);
        this->_mqtt_uri[length] = '\0';
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
            doc["mqtt_uri"] = this->_mqtt_uri;
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
            if (!doc.containsKey("mqtt_uri")) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }
            if (this->UpdateMqttUri(doc["mqtt_uri"])) {
                result = PROTO_INVAL_JSON_VALUE;
                break;
            }

            result = PROTO_NO_ERROR;

        } while(0);

        return result;
    }

private:
    char _mqtt_uri[256] = {0};
};
#endif /* MQTTURI_PROTO_H */

/**
 * @file WaterLevelProtobuf.h
 * @brief Auto-generated header file generated.
 */

#ifndef WATERLEVEL_PROTO_H
#define WATERLEVEL_PROTO_H

#include "stdint.h"
#include "string.h"
#include "Libraries/JSON/ArduinoJson/ArduinoJson.h"
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
            return -1;
        }

        uint16_t deserialized_min_size = sizeof(this->_timestamp) + sizeof(this->_value) + 0;

        if (in_buffer_size < deserialized_min_size) {
            return -3;
        }

        uint16_t offset = 0;
        memcpy(&this->_timestamp, &in_buffer[offset], sizeof(this->_timestamp));
        offset += sizeof(this->_timestamp);
        memcpy(&this->_value, &in_buffer[offset], sizeof(this->_value));

        return 0;
    }

    int32_t SerializeJson(char* out_buffer, uint16_t out_buffer_size) {
        uint32_t response_length = 0;

        do {
            if (out_buffer == nullptr) {
                break;
            }

            StaticJsonDocument<512> doc;         
            doc["timestamp"] = this->_timestamp;         
            doc["value"] = this->_value;
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
            if (!doc.containsKey("timestamp")) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }
            if (!doc.containsKey("value")) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }
            if (this->UpdateTimestamp(doc["timestamp"])) {
                result = PROTO_INVAL_JSON_VALUE;
                break;
            }
            if (this->UpdateValue(doc["value"])) {
                result = PROTO_INVAL_JSON_VALUE;
                break;
            }

            result = PROTO_NO_ERROR;

        } while(0);

        return result;
    }

private:
    uint64_t _timestamp = 0;
    uint32_t _value = 0;
};
#endif /* WATERLEVEL_PROTO_H */

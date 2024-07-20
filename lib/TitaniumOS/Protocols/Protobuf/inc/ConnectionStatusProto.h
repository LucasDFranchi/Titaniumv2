/**
 * @file ConnectionStatusProtobuf.h
 * @brief Auto-generated header file generated.
 */

#ifndef CONNECTIONSTATUS_PROTO_H
#define CONNECTIONSTATUS_PROTO_H

#include "stdint.h"
#include "string.h"
#include "Libraries/JSON/ArduinoJson/ArduinoJson.h"
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
            return -1;
        }

        uint16_t deserialized_min_size = sizeof(this->_ap_status) + sizeof(this->_sta_status) + 0;

        if (in_buffer_size < deserialized_min_size) {
            return -3;
        }

        uint16_t offset = 0;
        memcpy(&this->_ap_status, &in_buffer[offset], sizeof(this->_ap_status));
        offset += sizeof(this->_ap_status);
        memcpy(&this->_sta_status, &in_buffer[offset], sizeof(this->_sta_status));

        return 0;
    }

    int32_t SerializeJson(char* out_buffer, uint16_t out_buffer_size) {
        uint32_t response_length = 0;

        do {
            if (out_buffer == nullptr) {
                break;
            }

            StaticJsonDocument<512> doc;         
            doc["ap_status"] = this->_ap_status;         
            doc["sta_status"] = this->_sta_status;
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
            if (!doc.containsKey("ap_status")) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }
            if (!doc.containsKey("sta_status")) {
                result = PROTO_INVAL_JSON_KEY;
                break;
            }
            if (this->UpdateApStatus(doc["ap_status"])) {
                result = PROTO_INVAL_JSON_VALUE;
                break;
            }
            if (this->UpdateStaStatus(doc["sta_status"])) {
                result = PROTO_INVAL_JSON_VALUE;
                break;
            }

            result = PROTO_NO_ERROR;

        } while(0);

        return result;
    }

private:
    uint8_t _ap_status = 0;
    uint8_t _sta_status = 0;
};
#endif /* CONNECTIONSTATUS_PROTO_H */

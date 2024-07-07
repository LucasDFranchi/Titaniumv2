#ifndef MQTTCLIENT_PROTO_H
#define MQTTCLIENT_PROTO_H

#include "stdint.h"
#include "string.h"

class MQTTClientProtobuf {
public:
    MQTTClientProtobuf() = default;
    ~MQTTClientProtobuf() = default;

    static constexpr uint16_t BROKER_URI_SIZE = 128;

    const char* GetBrokerUri(void) const {  return this->_broker_uri; }

    int16_t GetSerializedSize(void) const {
        return ((strlen(this->_broker_uri) + 1));
    }

    int16_t GetMaxSize(void) const {
        return (sizeof(this->_broker_uri));
    }

    static int16_t GetStaticMaxSize(void) {
        return (BROKER_URI_SIZE);
    }

    int8_t UpdateBrokerUri(char* value) {
        if (value == nullptr) {
            return -1;
        }

        size_t value_length = strlen(value) + 1;

        if ((value_length == 0) || BROKER_URI_SIZE == 0) {
            return -2;
        }

        if (value_length > BROKER_URI_SIZE) {
            return -3;
        }

        memset(this->_broker_uri, 0, BROKER_URI_SIZE);
        memcpy(this->_broker_uri, value, value_length);

        return 0;
    }

    int16_t Serialize(char* out_buffer, uint16_t out_buffer_size) const {
        if (out_buffer == nullptr) {
            return 0;
        }

        uint16_t serialized_size = (strlen(this->_broker_uri) + 1);

        if (out_buffer_size < serialized_size) {
            return 0;
        }

        uint16_t offset = 0;

        memcpy(&out_buffer[offset], this->_broker_uri, strlen(this->_broker_uri) + 1);
    
        return serialized_size;
    }

    int8_t DeSerialize(const char* in_buffer, uint16_t in_buffer_size) {
        if (in_buffer == nullptr) {
            return -1;
        }

        uint16_t deserialized_min_size = 1;
        uint16_t deserialized_max_size = sizeof(this->_broker_uri);

        if (in_buffer_size < deserialized_min_size) {
            return -3;
        }

        if (in_buffer_size > deserialized_max_size) {
            return -3;
        }

        memset(this->_broker_uri, 0, BROKER_URI_SIZE);

        uint16_t offset = 0;
        memcpy(this->_broker_uri, &in_buffer[offset], strlen(&in_buffer[offset]) + 1);
    
        return 0;
    }

private:
    char _broker_uri[128] = {0};
};

#endif /* MQTTCLIENT_PROTO_H */ 


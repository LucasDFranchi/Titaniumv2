#ifndef PROTOBUF_INTERFACE_H
#define PROTOBUF_INTERFACE_H

#include <stdint.h>

enum protobufs_errors {
  PROTO_NO_ERROR   = 0,
  PROTO_INVAL_PTR  = -1,
  PROTO_OVERFLOW   = -2,
  PROTO_INVAL_SIZE = -3,
  PROTO_INVAL_NUM_TOKEN = -4,
  PROTO_INVAL_JSON_KEY = -5,
  PROTO_INVAL_JSON_VALUE = -6,
  PROTO_INVAL_JSON_PARSE = -7,
};

class IProtobuf {
public:
    virtual ~IProtobuf() = default;

    virtual int16_t GetSerializedSize() const = 0;
    virtual int16_t GetMaxSize() const = 0;
    virtual int16_t Serialize(char* out_buffer, uint16_t out_buffer_size) const = 0;
    virtual int8_t DeSerialize(const char* in_buffer, uint16_t in_buffer_size) = 0;
    virtual int32_t SerializeJson(char* out_buffer, uint16_t out_buffer_size) = 0;
    virtual int8_t DeSerializeJson(const char* in_buffer, uint16_t in_buffer_size) = 0;
};

#endif /* PROTOBUF_INTERFACE_H */

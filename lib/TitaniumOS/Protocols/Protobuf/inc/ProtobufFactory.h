
#ifndef PROTOBUF_FACTORY_H
#define PROTOBUF_FACTORY_H

#include "esp_log.h"
#include <memory>

#include "ProtobufIndex.h"
#include "CredentialsProto.h"
#include "CommunicationProto.h"
#include "ConnectionStatusProto.h"
#include "WaterLevelProto.h"
#include "MQTTUriProto.h"

class ProtobufFactory {
   public:
    static std::unique_ptr<IProtobuf> CreateProtobuf(uint16_t proto_index) {
        std::unique_ptr<IProtobuf> protobuf = nullptr;

        switch (proto_index) {

            case ProtobufIndex::CREDENTIALS: {
                protobuf = std::make_unique<CredentialsProtobuf>();
                break;
            }

            case ProtobufIndex::CONNECTIONSTATUS: {
                protobuf = std::make_unique<ConnectionStatusProtobuf>();
                break;
            }

            case ProtobufIndex::UART_TRANSMIT: {
                protobuf = std::make_unique<CommunicationProtobuf>();
                break;
            }

            case ProtobufIndex::UART_RECEIVE: {
                protobuf = std::make_unique<CommunicationProtobuf>();
                break;
            }

            case ProtobufIndex::LORA_TRANSMIT: {
                protobuf = std::make_unique<CommunicationProtobuf>();
                break;
            }

            case ProtobufIndex::LORA_RECEIVE: {
                protobuf = std::make_unique<CommunicationProtobuf>();
                break;
            }

            case ProtobufIndex::MQTTURI: {
                protobuf = std::make_unique<MQTTUriProtobuf>();
                break;
            }

            case ProtobufIndex::WATERLEVEL: {
                protobuf = std::make_unique<WaterLevelProtobuf>();
                break;
            }

            default: {
                ESP_LOGE("ProtobufFactory", "Invalid proto_index: %d", proto_index);
                break;
            }
        }

        return protobuf;
    }
};

#endif /* PROTOBUF_FACTORY_H */
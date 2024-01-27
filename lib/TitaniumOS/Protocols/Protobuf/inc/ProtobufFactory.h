#ifndef PROTOBUF_FACTORY_H
#define PROTOBUF_FACTORY_H

#include "CommunicationProto.h"
#include "ConnectionStatusProto.h"
#include "CredentialsProto.h"
#include "IProtobuf.h"
#include "MQTTClientProto.h"
#include "ProtobufIndex.h"
#include "WaterLevelProto.h"
#include "esp_log.h"
#include <memory>

class ProtobufFactory {
   public:
    static std::unique_ptr<IProtobuf> CreateProtobuf(uint16_t proto_index) {
        std::unique_ptr<IProtobuf> protobuf = nullptr;

        switch (proto_index) {
            case ProtobufIndex::CREDENTIALS: {
                protobuf = std::make_unique<CredentialsProtobuf>();
                break;
            }
            case ProtobufIndex::CONNECTION: {
                protobuf = std::make_unique<ConnectionStatusProtobuf>();
                break;
            }
            case ProtobufIndex::UART_TRANSMIT:
            case ProtobufIndex::LORA_TRANSMIT: {
                protobuf = std::make_unique<CommunicationProtobuf>();
                break;
            }
            case ProtobufIndex::MQTT: {
                protobuf = std::make_unique<MQTTClientProtobuf>();
                break;
            }
            case ProtobufIndex::WATER_LEVEL: {
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
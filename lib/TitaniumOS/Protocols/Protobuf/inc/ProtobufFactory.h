
#ifndef PROTOBUF_FACTORY_H
#define PROTOBUF_FACTORY_H

#include "esp_log.h"
#include <memory>

#include "CredentialsProto.h"
#include "ConnectionStatusProto.h"
#include "CommunicationProto.h"
#include "CommunicationProto.h"
#include "CommunicationProto.h"
#include "CommunicationProto.h"
#include "MQTTUriProto.h"
#include "WaterLevelProto.h"


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

            case ProtobufIndex::UART_TX: {
                protobuf = std::make_unique<CommunicationProtobuf>();
                break;
            }

            case ProtobufIndex::UART_RX: {
                protobuf = std::make_unique<CommunicationProtobuf>();
                break;
            }

            case ProtobufIndex::LORA_TX: {
                protobuf = std::make_unique<CommunicationProtobuf>();
                break;
            }

            case ProtobufIndex::LORA_RX: {
                protobuf = std::make_unique<CommunicationProtobuf>();
                break;
            }

            case ProtobufIndex::MQTT_URI: {
                protobuf = std::make_unique<MQTTUriProtobuf>();
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
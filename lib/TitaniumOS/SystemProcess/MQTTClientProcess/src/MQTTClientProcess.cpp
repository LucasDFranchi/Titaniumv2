#include "SystemProcess/MQTTClientProcess/inc/MQTTClientProcess.h"
#include "CustomProcess/WaterLevelProcess/inc/WaterLevelProto.h"
#include "SystemProcess/CommunicationProcess/inc/CommunicationProto.h"
#include "SystemProcess/NetworkProcess/inc/ConnectionStatusProto.h"
#include "SystemProcess/NetworkProcess/inc/CredentialsProto.h"
#include "SystemProcess/ProcessAreasIndex.h"

#include "esp_log.h"
#include "mqtt_client.h"

static const char *TAG = "mqtt_example";

static void mqtt_event_handler(void *arg, esp_event_base_t base, int32_t event_id, void *event_data) {
    auto event  = reinterpret_cast<esp_mqtt_event_handle_t>(event_data);
    auto client = event->client;
    // auto mqtt_client_process = reinterpret_cast<MQTTClientProcess *>(arg);

    switch (event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            esp_mqtt_client_subscribe(client, "titanium_area/1", 0);
            esp_mqtt_client_subscribe(client, "titanium_area/2", 0);
            esp_mqtt_client_subscribe(client, "titanium_area/3", 0);
            esp_mqtt_client_subscribe(client, "titanium_area/4", 0);
            esp_mqtt_client_subscribe(client, "titanium_area/5", 0);
            esp_mqtt_client_subscribe(client, "titanium_area/6", 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

esp_err_t MQTTClientProcess::Initialize(void) {
    auto result                       = ESP_OK;
    esp_mqtt_client_config_t mqtt_cfg = {};

    this->_shared_memory_manager.reset(SharedMemoryManager::GetInstance());

    this->_client = esp_mqtt_client_init(&mqtt_cfg);
    result += esp_mqtt_client_register_event(this->_client, MQTT_EVENT_ANY, mqtt_event_handler, this->_client);
    result += esp_mqtt_client_set_uri(this->_client, "mqtt://mqtt.eclipseprojects.io");
    result += esp_mqtt_set_config(this->_client, &mqtt_cfg);

    return result;
}

/**
 * @brief Main execution loop for HTTPServerProcess.
 *
 * This function runs an infinite loop with a delay of 1000 milliseconds.
 */
void MQTTClientProcess::Execute(void) {
    if (this->Initialize() != ESP_OK) {
        vTaskDelete(this->_process_handler);
    }

    while (1) {
        if (this->_shared_memory_manager.get()->IsAreaDataUpdated(ProcessAreaIndex::CONNECTION)) {
            this->_shared_memory_manager.get()->Read(ProcessAreaIndex::CONNECTION,
                                                     &this->_connection_status);
        }

        do {
            auto ap_changed =
                this->_last_connection_status.GetApStatus() !=
                this->_connection_status.GetApStatus();
            auto sta_changed =
                this->_last_connection_status.GetStaStatus() !=
                this->_connection_status.GetStaStatus();

            for (uint8_t i = 0; i < this->_shared_memory_manager->GetNumAreas(); i++) {
                this->PublishMemoryArea(i);
            }

            if (!ap_changed && !sta_changed) {
                break;
            }

            auto ap_status  = this->_connection_status.GetApStatus();
            auto sta_status = this->_connection_status.GetStaStatus();

            if ((ap_status == NetworkStatus::CONNECTED) || (sta_status == NetworkStatus::CONNECTED)) {
                if (this->_client_status != NetworkStatus::CONNECTED) {
                    this->StartMQTTClient();
                    this->_client_status = NetworkStatus::CONNECTED;
                    ESP_LOGI(TAG, "MQTT CLIENT STARTED");
                }
            } else if ((ap_status == NetworkStatus::NOT_CONNECTED) && (sta_status == NetworkStatus::NOT_CONNECTED)) {
                if (this->_client_status != NetworkStatus::CONNECTED) {
                    this->StopMQTTClient();
                    this->_client_status = NetworkStatus::NOT_CONNECTED;
                    ESP_LOGI(TAG, "MQTT CLIENT STOPED");
                }
            }
        } while (0);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Starts the HTTP server and registers URI handlers.
 *
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t MQTTClientProcess::StartMQTTClient(void) {
    return esp_mqtt_client_start(this->_client);
}

/**
 * @brief Stops the HTTP server.
 *
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t MQTTClientProcess::StopMQTTClient(void) {
    return esp_mqtt_client_stop(this->_client);
}

esp_err_t MQTTClientProcess::PublishMemoryArea(uint8_t area_index) {
    char response_buffer[512] = {0};
    char topic_area[64]       = {0};
    uint16_t response_size    = 0;
    esp_err_t result          = ESP_FAIL;

    switch (area_index) {
        case ProcessAreaIndex::CONNECTION: {
            ConnectionStatusProtobuf protobuf{};
            this->_shared_memory_manager->Read<ConnectionStatusProtobuf>(area_index, &protobuf);
            response_size = protobuf.SerializeJson(response_buffer, sizeof(response_buffer));
        } break;
        case ProcessAreaIndex::CREDENTIALS: {
            CredentialsProtobuf protobuf{};
            this->_shared_memory_manager->Read<CredentialsProtobuf>(area_index, &protobuf);
            response_size = protobuf.SerializeJson(response_buffer, sizeof(response_buffer));
        } break;
        case ProcessAreaIndex::UART_TRANSMIT:
        case ProcessAreaIndex::LORA_TRANSMIT: {
            CommunicationProtobuf protobuf{};
            this->_shared_memory_manager->Read<CommunicationProtobuf>(area_index, &protobuf);
            response_size = protobuf.SerializeJson(response_buffer, sizeof(response_buffer));
        } break;
        case CustomProcessAreaIndex::WATER_LEVEL: {
            WaterLevelProtobuf protobuf{};
            this->_shared_memory_manager->Read<WaterLevelProtobuf>(area_index, &protobuf);
            response_size = protobuf.SerializeJson(response_buffer, sizeof(response_buffer));
        } break;
        default: {
            response_size = 0;
        }
    }

    if (response_size > 0) {
        if (snprintf(topic_area, sizeof(topic_area), "titanium_area/%d", area_index) > 0) {
            esp_mqtt_client_publish(this->_client, topic_area, response_buffer, 0, 1, 0);
            result = ESP_OK;
        }
    }

    return result;
}
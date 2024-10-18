#include "SystemProcess/MQTTClientProcess/inc/MQTTClientProcess.h"

#include "esp_log.h"
#include "mbedtls/base64.h"
#include "mqtt_client.h"

static const char *TAG = "mqtt_example";

static void mqtt_event_handler(void *arg, esp_event_base_t base, int32_t event_id, void *event_data) {
    auto event = reinterpret_cast<esp_mqtt_event_handle_t>(event_data);

    switch (event_id) {
        case MQTT_EVENT_CONNECTED: {
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            // mqtt_client_process->SubscribeMemoryArea();
            break;
        }
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

titan_err_t MQTTClientProcess::Initialize(void) {
    auto result                       = ESP_OK;
    esp_mqtt_client_config_t mqtt_cfg = {};

    this->_shared_memory_manager.reset(SharedMemoryManager::GetInstance());

    this->_client = esp_mqtt_client_init(&mqtt_cfg);
    result += esp_mqtt_client_register_event(this->_client, MQTT_EVENT_ANY, mqtt_event_handler, this->_client);
    result += esp_mqtt_client_set_uri(this->_client, "mqtt://mqtt.eclipseprojects.io");
    result += esp_mqtt_set_config(this->_client, &mqtt_cfg);

    this->_connection_status.ap_connected       = NETWORK_STATUS_DISCONNECTED;
    this->_connection_status.sta_connected      = NETWORK_STATUS_DISCONNECTED;
    this->_last_connection_status.ap_connected  = NETWORK_STATUS_DISCONNECTED;
    this->_last_connection_status.sta_connected = NETWORK_STATUS_DISCONNECTED;

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
        do {
            this->_shared_memory_manager.get()->Read(MEMORY_AREAS_NETWORK_INFORMATION,
                                                     this->_connection_status,
                                                     network_information_t_msg,
                                                     true);

            auto ap_changed =
                this->_last_connection_status.ap_connected !=
                this->_connection_status.ap_connected;
            auto sta_changed =
                this->_last_connection_status.sta_connected !=
                this->_connection_status.sta_connected;

            if (this->_client_status == NETWORK_STATUS_CONNECTED) {
                for (uint8_t i = 1; i < this->_shared_memory_manager->GetNumAreas(); i++) {
                    this->PublishMemoryArea(i);
                }
            }

            if (!ap_changed && !sta_changed) {
                break;
            }

            this->_last_connection_status.ap_connected  = this->_connection_status.ap_connected;
            this->_last_connection_status.sta_connected = this->_connection_status.sta_connected;

            auto ap_status  = this->_connection_status.ap_connected;
            auto sta_status = this->_connection_status.sta_connected;

            if ((ap_status == NETWORK_STATUS_CONNECTED) || (sta_status == NETWORK_STATUS_CONNECTED)) {
                if (this->_client_status != NETWORK_STATUS_CONNECTED) {
                    this->StartMQTTClient();
                    this->_client_status = NETWORK_STATUS_CONNECTED;
                    ESP_LOGI(TAG, "MQTT CLIENT STARTED");

                    this->SubscribeMemoryArea();
                }
            } else if ((ap_status == NETWORK_STATUS_DISCONNECTED) && (sta_status == NETWORK_STATUS_DISCONNECTED)) {
                if (this->_client_status == NETWORK_STATUS_CONNECTED) {
                    this->StopMQTTClient();
                    this->_client_status = NETWORK_STATUS_DISCONNECTED;
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
titan_err_t MQTTClientProcess::StartMQTTClient(void) {
    auto result = Error::NO_ERROR;
    ESP_LOGI(TAG, "StartMQTTClient");
    if (this->_is_mqtt_started == false) {
        result                 = esp_mqtt_client_start(this->_client);
        this->_is_mqtt_started = true;
    }

    return result;
}

/**
 * @brief Stops the HTTP server.
 *
 * @return ESP_OK on success, or an error code on failure.
 */
titan_err_t MQTTClientProcess::StopMQTTClient(void) {
    auto result = Error::NO_ERROR;
    if (this->_is_mqtt_started == true) {
        result                 = esp_mqtt_client_stop(this->_client);
        this->_is_mqtt_started = false;
    }

    return result;
}

titan_err_t MQTTClientProcess::PublishMemoryArea(uint8_t area_index) {
    char response_buffer[512] = {0};
    char topic_area[64]       = {0};
    titan_err_t result        = Error::UNKNOW_FAIL;

    do {
        if (!this->_shared_memory_manager->ShouldTransmitArea(area_index)) {
            result = Error::NO_ERROR;
            break;
        }

        if (!this->_shared_memory_manager->IsAreaDataUpdated(area_index)) {
            result = Error::NO_ERROR;
            break;
        }

        char raw_response_buffer[this->_shared_memory_manager->GetAreaSize(area_index)] = {0};

        auto read_bytes = this->_shared_memory_manager->Read(area_index,
                                                             raw_response_buffer,
                                                             sizeof(raw_response_buffer),
                                                             true);

        if (read_bytes == 0) {
            result = Error::READ_FAIL;
            break;
        }

        if (snprintf(topic_area, sizeof(topic_area), "titanium_area/%d", area_index) < 0) {
            result = Error::TOPIC_MQTT_WRITE_FAIL;
            break;
        }
        size_t outlen      = 0;
        auto encode_result = mbedtls_base64_encode(reinterpret_cast<uint8_t *>(response_buffer),
                                                   sizeof(response_buffer),
                                                   &outlen,
                                                   reinterpret_cast<uint8_t *>(raw_response_buffer),
                                                   read_bytes);
        
        if (encode_result != Error::NO_ERROR) {
            result = Error::ENCODE_BASE64_FAIL;
            break;
        }

        auto publish_result = esp_mqtt_client_publish(this->_client, topic_area, response_buffer, 0, 1, 0);

        if (publish_result != Error::NO_ERROR) {
            result = Error::MQTT_PUBLISH_FAIL;
            break;
        }

        result = Error::NO_ERROR;

    } while (0);

    return result;
}

titan_err_t MQTTClientProcess::SubscribeMemoryArea(void) {
    char topic_area[64] = {0};
    titan_err_t result  = Error::UNKNOW_FAIL;

    do {
        if (this->_shared_memory_manager == nullptr) {
            break;
        }

        if (this->_client == nullptr) {
            break;
        }

        for (uint8_t i = 0; i < this->_shared_memory_manager->GetNumAreas(); i++) {
            if (snprintf(topic_area, sizeof(topic_area), "titanium_area/%d", i) < 0) {
                esp_mqtt_client_subscribe(this->_client, topic_area, 0);
            }
        }
        result = ESP_OK;

    } while (0);

    return result;
}
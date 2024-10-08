#include "SystemProcess/MQTTClientProcess/inc/MQTTClientProcess.h"

#include "esp_log.h"
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

    this->_connection_status.ap_connected = NETWORK_STATUS_DISCONNECTED;
    this->_connection_status.sta_connected = NETWORK_STATUS_DISCONNECTED;
    this->_last_connection_status.ap_connected = NETWORK_STATUS_DISCONNECTED;
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

            this->_last_connection_status.ap_connected = this->_connection_status.ap_connected;
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

        auto read_bytes = this->_shared_memory_manager->Read(area_index,
                                                    response_buffer, 
                                                    sizeof(response_buffer),
                                                    true);

        if (read_bytes == 0) {
            return result;
        }

        if (snprintf(topic_area, sizeof(topic_area), "titanium_area/%d", area_index) < 0) {
            break;
        }

        esp_mqtt_client_publish(this->_client, topic_area, response_buffer, 0, 1, 0);
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
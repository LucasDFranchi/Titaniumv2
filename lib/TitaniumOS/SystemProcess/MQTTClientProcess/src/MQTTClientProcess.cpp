#include "SystemProcess/MQTTClientProcess/inc/MQTTClientProcess.h"
#include "Libraries/JSON/ArduinoJson/ArduinoJson.h"
#include "SystemProcess/NetworkProcess/inc/NetworkUtils.hpp"

#include "esp_log.h"
#include "mbedtls/base64.h"
#include "mqtt_client.h"

static const char *TAG = "MQTT Client Process";

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

    this->_sta_status.status = NETWORK_STATUS_DISCONNECTED;

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
        this->_shared_memory_manager.get()->Read(MEMORY_AREAS_STATION_STATUS,
                                                 this->_sta_status,
                                                 station_status_t_msg,
                                                 true);

        if (this->_sta_status.status == NETWORK_STATUS_CONNECTED) {
            this->StartMQTTClient();
            this->SubscribeMemoryArea();
            for (uint8_t i = 1; i < this->_shared_memory_manager->GetNumAreas(); i++) {
                this->PublishMemoryArea(i);
            }
        } else if (this->_sta_status.status == NETWORK_STATUS_DISCONNECTED) {
            this->StopMQTTClient();
        }
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
    if (this->_is_mqtt_started == false) {
        ESP_LOGI(TAG, "MQTT CLIENT STARTED");
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
        ESP_LOGI(TAG, "MQTT CLIENT STOPED");
        result                 = esp_mqtt_client_stop(this->_client);
        this->_is_mqtt_started = false;
    }

    return result;
}

/**
 * Publishes the data from a specified memory area to an MQTT topic.
 *
 * This function checks if the specified memory area should be transmitted
 * and whether its data has been updated. If so, it reads the data from the
 * shared memory, encodes it in Base64, and publishes it to the MQTT topic
 * corresponding to the memory area.
 *
 * @param area_index The index of the memory area to publish.
 * @return titan_err_t An error code indicating the result of the operation:
 *         - Error::NO_ERROR if the data was successfully published,
 *         - Error::UNKNOW_FAIL if an unknown error occurred,
 *         - Error::READ_FAIL if there was an error reading from shared memory,
 *         - Error::TOPIC_MQTT_WRITE_FAIL if there was an error writing the topic,
 *         - Error::ENCODE_BASE64_FAIL if there was an error encoding data in Base64.
 */
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

        result = this->PublishPackage(topic_area, response_buffer);

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

        if (!this->_should_subscribe) {
            break;
        }

        for (uint8_t i = 0; i < this->_shared_memory_manager->GetNumAreas(); i++) {
            if (snprintf(topic_area, sizeof(topic_area), "titanium_area/%d", i) < 0) {
                esp_mqtt_client_subscribe(this->_client, topic_area, 0);
            }
        }
        result                  = ESP_OK;
        this->_should_subscribe = true;

    } while (0);

    return result;
}

/**
 * @brief Publishes raw data to an MQTT topic by encoding it into a JSON package.
 *
 * This function takes raw data, encodes it into a JSON object, and publishes the resulting
 * JSON string to the specified MQTT topic. The `raw_data` is stored under the key `"raw_data"`
 * in the JSON object. If serialization or MQTT publishing fails, appropriate error codes
 * are returned.
 *
 * @param[in] topic     The MQTT topic to which the message will be published.
 * @param[in] raw_data  The raw data to be packaged into the JSON object and transmitted.
 *
 * @return titan_err_t  Error code indicating the result of the operation:
 *                      - Error::NO_ERROR: The package was successfully published.
 *                      - Error::SERIALIZE_JSON_ERROR: JSON serialization failed.
 *                      - Error::MQTT_PUBLISH_FAIL: MQTT publish operation failed.
 */
titan_err_t MQTTClientProcess::PublishPackage(const char *topic, const char *raw_data) {
    StaticJsonDocument<512> json_document;
    char response_buffer_json[512] = {0};

    json_document["id"]       = 1;
    json_document["raw_data"] = raw_data;

    size_t bytes_written = serializeJson(json_document, response_buffer_json, sizeof(response_buffer_json));

    if (bytes_written == 0) {
        ESP_LOGE("MQTT Client", "Failed to serialize JSON");
        return Error::SERIALIZE_JSON_ERROR;
    }

    auto publish_result = esp_mqtt_client_publish(this->_client, topic, response_buffer_json, 0, 1, 0);

    if (publish_result != Error::NO_ERROR) {
        return Error::MQTT_PUBLISH_FAIL;
    }

    return Error::NO_ERROR;
}
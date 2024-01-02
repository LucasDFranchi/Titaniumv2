#include "NetworkManager.h"
#include "memory/MemoryManager.hpp"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if ((event_base != WIFI_EVENT) && (event_base != IP_EVENT))
        return;

    NetworkManager * network_pointer_object = static_cast<NetworkManager *>(arg);

    switch (event_id){
        case WIFI_EVENT_STA_START:
        case WIFI_EVENT_STA_DISCONNECTED:
            network_pointer_object->SetWiFiConnection_(false);
            network_pointer_object->result = esp_wifi_connect();
        break;
        case IP_EVENT_STA_GOT_IP:
            network_pointer_object->SetWiFiConnection_(true);
        break;
        default:
        break;
    }
}

/**
 * @brief Initializes the network manager by configuring the Wi-Fi settings.
 *
 * This function initializes the network manager by setting up Wi-Fi, registering event handlers,
 * and connecting to a Wi-Fi network using the credentials stored in the memory manager.
 *
 * @return An `esp_err_t` value, which is the result of the initialization process.
 */
esp_err_t NetworkManager::Initialize_(void){
    esp_err_t result = ESP_OK;
    auto memory_manager = MemoryManager::GetInstance();

    memory_manager->Read(CREDENTIALS_AREA, &this->cred_area_);

    result += esp_netif_init();
    result += esp_event_loop_create_default();
    this->esp_netif_pointer_ = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    result += esp_wifi_init(&cfg);

    result += this->RegisterWiFiEvents_();

    wifi_config_t wifi_config;

    memcpy_s(wifi_config.sta.ssid, this->cred_area_.sta_ssid, sizeof(wifi_config.sta.ssid));  
    memcpy_s(wifi_config.sta.password, this->cred_area_.sta_password, sizeof(wifi_config.sta.password));  

    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    result += esp_wifi_set_mode(WIFI_MODE_STA);
    result += esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    result += esp_wifi_start();
    return result;
}

/**
 * @brief Executes the network manager functionality in an infinite loop.
 *
 * This function runs an infinite loop with a delay of 50 milliseconds. It initializes the network,
 * and writes the connection status to the memory manager at regular intervals.
 */
void NetworkManager::Execute(void){
    auto memory_manager = MemoryManager::GetInstance();

    if (this->Initialize_() != ESP_OK){
        vTaskDelete(this->process_handler_);
    }

    while(1){
        memory_manager->Write(CONNECTION_AREA, sizeof(this->connection_area_), &this->connection_area_);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Registers WiFi and IP event handlers.
 *
 * This function registers event handlers for WiFi and IP events to handle connection events.
 *
 * @return An `esp_err_t`, which is a type defined in the ESP-IDF framework for error handling.
 */
esp_err_t NetworkManager::RegisterWiFiEvents_(void){
    esp_err_t result = ESP_OK;

    result += esp_event_handler_instance_register(
                                        WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &event_handler,
                                        this,
                                        nullptr);

    result += esp_event_handler_instance_register(
                                        IP_EVENT,
                                        IP_EVENT_STA_GOT_IP,
                                        &event_handler,
                                        this,
                                        nullptr);

    return result;
}

/**
 * @brief Sets the status of the WiFi connection.
 *
 * This function sets the status of the WiFi connection based on the provided status parameter.
 *
 * @param status A boolean value that represents the desired connection status of the WiFi network.
 */
void NetworkManager::SetWiFiConnection_(uint8_t status){
    this->connection_area_.connection_status = status;
}

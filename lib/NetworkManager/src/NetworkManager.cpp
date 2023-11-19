#include "NetworkManager.h"
#include "MemoryManager.h"
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
 * The function initializes the network manager by configuring the Wi-Fi settings.
 * 
 * @return an `esp_err_t` value, which is the result of the initialization process.
 */
esp_err_t NetworkManager::Initialize_(void){
    esp_err_t result = ESP_OK;
    auto memory_manager = MemoryManager::GetInstance();

    result += esp_netif_init();
    result += esp_event_loop_create_default();
    this->esp_netif_pointer_ = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    result += esp_wifi_init(&cfg);

    printf("Wifi Status %d Result End\n", result);
    result += this->RegisterWiFiEvents_();

    wifi_config_t wifi_config;
    uint16_t size_ssid = 0;
    uint16_t size_password = 0;
    
    memory_manager->Read(SSID_AREA, &size_ssid, wifi_config.sta.ssid);
    memory_manager->Read(PASSWORD_AREA, &size_password, wifi_config.sta.password);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    result += esp_wifi_set_mode(WIFI_MODE_STA);
    result += esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    result += esp_wifi_start();
    return result;
}

/**
 * The function `Execute` runs an infinite loop with a delay of 50 milliseconds.
 */
void NetworkManager::Execute(void){
    auto memory_manager = MemoryManager::GetInstance();

    this->Initialize_();

    while(1){
        memory_manager->Write(CONNECTION_AREA, sizeof(this->connected_), &this->connected_);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * The function `RegisterWiFiEvents_` registers event handlers for WiFi and IP events.
 * 
 * @return an esp_err_t, which is a type defined in the ESP-IDF framework for error handling.
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
 * The function sets the status of the WiFi connection.
 * 
 * @param status The status parameter is a boolean value that represents the desired connection status
 * of the WiFi network.
 */
void NetworkManager::SetWiFiConnection_(uint8_t status){
    this->connected_ = status;
}

/**
 * @file NetworkManager.h
 * @brief Definition of the NetworkManager class for handling Wi-Fi configuration and events.
 */

#include "NetworkManager.h"
#include "memory/MemoryManager.hpp"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"

static const char* TAG = "NetworkManager";

/**
 * @brief Namespace containing constants related to the Access Point (AP) configuration.
 */
namespace AP {
    const uint8_t          ssid[] = "Titanium";
    const uint8_t          password[] = "root1234";
    const uint8_t          channel = 1;
    const uint8_t          visibility = 0;
    const uint8_t          max_connections = 1;
    const char*            ip = "192.168.0.1";
    const char*            gw = "192.168.0.1";
    const char*            netmask = "255.255.255.0";
    const uint8_t          beacon_interval = 100;
    const wifi_bandwidth_t bw = WIFI_BW_HT20;
    const wifi_ps_type_t   power_save = WIFI_PS_MAX_MODEM;
}

/**
 * @brief Namespace containing constants related to the Station (STA) configuration.
 */
namespace STA {
    const uint8_t ssid_max_lenght = 32;
    const uint8_t password_max_lenght = 64;
    const uint8_t max_retries = 5;
}

/**
 * @brief Event handler for Wi-Fi-related events.
 *
 * This function handles events such as AP start/stop, station connected/disconnected,
 * and station got/disconnected from IP events.
 *
 * @param arg Pointer to user data (NetworkManager instance).
 * @param event_base Event base.
 * @param event_id Event ID.
 * @param event_data Pointer to event data.
 */
static void WiFiAppEventHandler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    esp_err_t ret = ESP_OK;

    if (event_base == WIFI_EVENT){
        switch (event_id){
            case WIFI_EVENT_AP_START:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
                break;
            case WIFI_EVENT_AP_STOP:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STOP");
                break;
            case WIFI_EVENT_AP_STACONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
                break;
            case WIFI_EVENT_AP_STADISCONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
                break;
            case WIFI_EVENT_STA_START:
                ret =  esp_wifi_connect();
                ESP_LOGI(TAG, "WIFI_EVENT_STA_START %d", ret);
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ret =  esp_wifi_connect();
                ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED %d", ret);
                break;
        }
    }
    else if (event_base == IP_EVENT){
        switch (event_id) {
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
            break;
        }
    }
}

/**
     * @brief Initializes the NetworkManager instance.
     * @return ESP_OK on success, or an error code on failure.
     */
esp_err_t NetworkManager::Initialize(void){
    esp_err_t result = ESP_OK;
    this->_memory_manager = MemoryManager::GetInstance();

    // esp_log_level_set("wifi", ESP_LOG_NONE);

    result += this->RegisterWiFiEvents();
    result += esp_netif_init();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    result += esp_wifi_init(&cfg);
    result += esp_wifi_set_storage(WIFI_STORAGE_RAM);

    this->_esp_netif_sta = esp_netif_create_default_wifi_sta();
    this->_esp_netif_ap  = esp_netif_create_default_wifi_ap();

    wifi_config_t ap_config;
    wifi_config_t sta_config;

    result += esp_wifi_set_mode(WIFI_MODE_APSTA);
    result += this->SetAccessPointMode(&ap_config);
    result += this->SetStationMode(&sta_config);

    result += esp_wifi_start();
    return result;
}

/**
 * @brief Executes the main functionality of the NetworkManager.
 * Continuously writes connection status to memory.
 */
void NetworkManager::Execute(void){
    auto memory_manager = MemoryManager::GetInstance();

    if (this->Initialize() != ESP_OK){
        vTaskDelete(this->_process_handler);
    }

    while(1){
        if (this->_last_connection != this->_connection_area.connection_status){
            memory_manager->Write(CONNECTION_AREA, sizeof(this->_connection_area), &this->_connection_area);
            this->_last_connection = this->_connection_area.connection_status;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Registers Wi-Fi-related events with the event loop.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t NetworkManager::RegisterWiFiEvents(void){
    esp_err_t result = ESP_OK;

    result += esp_event_loop_create_default();

    result += esp_event_handler_instance_register(
                                        WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &WiFiAppEventHandler,
                                        this,
                                        nullptr);

    result += esp_event_handler_instance_register(
                                        IP_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &WiFiAppEventHandler,
                                        this,
                                        nullptr);

    return result;
}

 /**
 * @brief Sets the Station (STA) mode configuration.
 * @param sta_config Pointer to the Wi-Fi configuration structure for STA mode.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t NetworkManager::SetStationMode(wifi_config_t* sta_config){
    auto result = ESP_OK;

    memset_s(reinterpret_cast<uint8_t*>(sta_config), 0, sizeof(wifi_config_t));

    this->SetCredentials(sta_config);

    sta_config->sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    sta_config->sta.pmf_cfg.capable = true;
    sta_config->sta.pmf_cfg.required = false;

    result += esp_wifi_set_config(WIFI_IF_STA, sta_config);

    return result;
}

/**
 * @brief Sets the Access Point (AP) mode configuration.
 * @param ap_config Pointer to the Wi-Fi configuration structure for AP mode.
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t NetworkManager::SetAccessPointMode(wifi_config_t* ap_config){
    auto result = ESP_OK;

    memset_s(reinterpret_cast<uint8_t*>(ap_config), 0, sizeof(wifi_config_t));
    memcpy_s(ap_config->ap.ssid, const_cast<uint8_t*>(AP::ssid), sizeof(AP::ssid));  
    memcpy_s(ap_config->ap.password, const_cast<uint8_t*>(AP::password), sizeof(AP::password));

    ap_config->ap.ssid_len        = sizeof(AP::ssid);  
    ap_config->ap.channel         = AP::channel;  
    ap_config->ap.ssid_hidden     = AP::visibility;  
    ap_config->ap.authmode        = WIFI_AUTH_WPA2_PSK;
    ap_config->ap.max_connection  = AP::max_connections;
    ap_config->ap.beacon_interval = AP::beacon_interval;

    esp_netif_ip_info_t ap_ip_info;
    memset_s(reinterpret_cast<uint8_t*>(&ap_ip_info), 0, sizeof(esp_netif_ip_info_t));

    esp_netif_dhcps_stop(this->_esp_netif_ap);
    inet_pton(AF_INET, AP::ip, &ap_ip_info.ip);
    inet_pton(AF_INET, AP::gw, &ap_ip_info.gw);
    inet_pton(AF_INET, AP::netmask, &ap_ip_info.netmask);

    result += esp_netif_set_ip_info(this->_esp_netif_ap, &ap_ip_info);
    result += esp_netif_dhcps_start(this->_esp_netif_ap);
    result += esp_wifi_set_config(WIFI_IF_AP, ap_config);
    result += esp_wifi_set_bandwidth(WIFI_IF_AP, AP::bw);
    result += esp_wifi_set_ps(AP::power_save);

    return result;
}

/**
 * @brief Sets Wi-Fi credentials in the provided Wi-Fi configuration structure.
 * @param wifi_config Pointer to the Wi-Fi configuration structure.
 */
void NetworkManager::SetCredentials(wifi_config_t* wifi_config){
    // WOKRAROUND
    uint8_t ssid[] = "NETPARQUE_PAOLA\0";
    uint8_t password[] = "NPQ196253\0";

    memcpy_s(wifi_config->sta.ssid, (uint8_t*)ssid, sizeof(wifi_config->sta.ssid));  
    memcpy_s(wifi_config->sta.password, (uint8_t*)password, sizeof(wifi_config->sta.password));

    // // WOKRAROUND
    // this->_memory_manager->Read(CREDENTIALS_AREA, &this->_cred_area);

    // memcpy_s(wifi_config.sta.ssid, this->_cred_area.sta_ssid, sizeof(wifi_config.sta.ssid));  
    // memcpy_s(wifi_config.sta.password, this->_cred_area.sta_password, sizeof(wifi_config.sta.password));  
}


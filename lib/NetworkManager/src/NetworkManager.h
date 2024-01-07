#ifndef NETWORK_MANAGER_GUARD
#define NETWORK_MANAGER_GUARD

#include "ProcessTemplate.h"
#include "memory/MemoryManager.hpp"
#include "memory/AreaDefinitions/CredentialsArea.h"
#include "memory/AreaDefinitions/ConnectionArea.h"

#include "esp_wifi.h"
#include "esp_err.h"

/**
 * @brief NetworkManager class for handling Wi-Fi configuration and events.
 */
class NetworkManager : public ProcessTemplate{
    public:
    NetworkManager(const char* name, uint32_t memory, UBaseType_t  priority) : ProcessTemplate(this, name, memory, priority, &this->_process_handler) { };
    void Execute(void);

    private:
    esp_err_t Initialize(void);
    esp_err_t RegisterWiFiEvents(void);
    esp_err_t SetStationMode(wifi_config_t* sta_config);
    esp_err_t SetAccessPointMode(wifi_config_t* ap_config);
    void      SetCredentials(wifi_config_t* wifi_config);

    private:
    MemoryManager* _memory_manager = nullptr;
    esp_netif_t*   _esp_netif_ap = nullptr;
    esp_netif_t*   _esp_netif_sta = nullptr;
    TaskHandle_t   _process_handler = nullptr;
    credentials_st _cred_area;
    connection_st  _connection_area;
    uint8_t        _last_connection = 0;
};

#endif /* NETWORK_MANAGER_GUARD */
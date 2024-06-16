#ifndef NETWORK_MANAGER_GUARD
#define NETWORK_MANAGER_GUARD

#include "Process/Template/ProcessTemplate.h"
#include "Managers/ManagersAreaIndex.h"
#include "HAL/memory/SharedMemoryManager.h"

#include "esp_wifi.h"
#include "esp_err.h"

typedef struct credentials_s {
    uint8_t sta_ssid[32];
    uint8_t sta_password[64];
} __attribute__((packed)) credentials_st;

typedef struct connection_s {
    uint8_t connection_ap_status;
    uint8_t connection_sta_status;
} __attribute__((packed)) connection_st;

namespace NetworkStatus {
    constexpr uint8_t NOT_CONNECTED = 0;
    constexpr uint8_t CONNECTED     = 1;
}  // namespace NetworkStatus

/**
 * @brief NetworkManager class for handling Wi-Fi configuration and events.
 */
class NetworkManager : public ProcessTemplate {
   public:
    NetworkManager(const char* name, uint32_t memory, UBaseType_t priority)
        : ProcessTemplate(this, name, memory, priority, &this->_process_handler){};
    void Execute(void);
    void SetAPConnection(uint8_t status);
    void SetSTAConnection(uint8_t status);

   private:
    esp_err_t Initialize(void);
    esp_err_t RegisterWiFiEvents(void);
    esp_err_t SetStationMode(wifi_config_t* sta_config);
    esp_err_t SetAccessPointMode(wifi_config_t* ap_config);
    void SetCredentials(wifi_config_t* wifi_config);

   public:
    esp_netif_t* _esp_netif_ap = nullptr;

   private:
    SharedMemoryManager* _shared_memory_manager = nullptr;
    esp_netif_t* _esp_netif_sta                 = nullptr;
    TaskHandle_t _process_handler               = nullptr;
    wifi_config_t _ap_config;
    wifi_config_t _sta_config;
    credentials_st _cred_area;
    connection_st _connection_area;
    uint8_t _need_update_network_data;
};

#endif /* NETWORK_MANAGER_GUARD */
#ifndef NETWORK_MANAGER_GUARD
#define NETWORK_MANAGER_GUARD

#include "HAL/memory/SharedMemoryManager.h"
#include "Managers/ManagersAreaIndex.h"
#include "Process/Template/ProcessTemplate.h"
#include "NetworkManagerDataModel.h"

#include "esp_err.h"
#include "esp_wifi.h"

/**
 * @brief NetworkManager class for handling Wi-Fi configuration and events.
 */
class NetworkManager : public ProcessTemplate {
   public:
    /**
     * @brief Constructor for NetworkManager.
     * @param[in] name Name of the process.
     * @param[in] memory Memory size for the process.
     * @param[in] priority Priority of the process.
     */
    NetworkManager(const char* name, uint32_t memory, UBaseType_t priority)
        : ProcessTemplate(this, name, memory, priority, &this->_process_handler){};
    void SetAPConnection(uint8_t status);
    void SetSTAConnection(uint8_t status);

   private:
    void Execute(void);
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
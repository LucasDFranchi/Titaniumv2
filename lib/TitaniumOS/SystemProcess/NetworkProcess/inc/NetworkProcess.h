#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "HAL/memory/SharedMemoryManager.h"
#include "SystemProcess/ProcessAreasIndex.h"
#include "NetworkProcessDataModel.h"
#include "SystemProcess/Template/ProcessTemplate.h"

#include "esp_err.h"
#include "esp_wifi.h"

/**
 * @brief NetworkProcess class for handling Wi-Fi configuration and events.
 */
class NetworkProcess : public ProcessTemplate {
   public:
    /**
     * @brief Constructor for NetworkProcess.
     *
     * @param[in] name Name of the process.
     * @param[in] memory Memory size for the process.
     * @param[in] priority Priority of the process.
     */
    NetworkProcess(const char* name, uint32_t memory, UBaseType_t priority)
        : ProcessTemplate(name, memory, priority, &this->_process_handler){};
    void SetAPConnection(uint8_t status);
    void SetSTAConnection(uint8_t status);

   private:
    void Execute(void);
    esp_err_t Initialize(void);
    esp_err_t RegisterWiFiEvents(void);
    esp_err_t SetStationMode(wifi_config_t* sta_config);
    esp_err_t SetAccessPointMode(wifi_config_t* ap_config);
    void SetCredentials(wifi_config_t* wifi_config);

   private:
    esp_netif_t* _esp_netif_ap                  = nullptr;  ///< ESP-IDF network interface for AP mode.
    SharedMemoryManager* _shared_memory_manager = nullptr;  ///< Instance of the SharedMemoryManager.
    esp_netif_t* _esp_netif_sta                 = nullptr;  ///< ESP-IDF network interface for STA mode.
    TaskHandle_t _process_handler               = nullptr;  ///< Task handle for the NetworkProcess process.
    wifi_config_t _ap_config;                               ///< Wi-Fi configuration structure for AP mode.
    wifi_config_t _sta_config;                              ///< Wi-Fi configuration structure for STA mode.
    credentials_st _cred_area;                              ///< Structure for credentials storage.
    connection_st _connection_area;                         ///< Structure for connection status storage.
    uint8_t _need_update_network_data;                      ///< Flag indicating if network data needs updating.
};

#endif /* NETWORK_MANAGER_GUARD */
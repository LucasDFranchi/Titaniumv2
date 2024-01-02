#ifndef NETWORK_MANAGER_GUARD
#define NETWORK_MANAGER_GUARD

#include "ProcessTemplate.h"
#include "memory/AreaDefinitions/CredentialsArea.h"
#include "memory/AreaDefinitions/ConnectionArea.h"

#include "esp_wifi.h"
#include "esp_err.h"

/**
 * A class that manages the graphic driver process.
 */
class NetworkManager : public ProcessTemplate{
    public:
    NetworkManager(const char* name, uint32_t memory, UBaseType_t  priority) : ProcessTemplate(this, name, memory, priority, &this->process_handler_) { };
    void Execute(void);
    void SetWiFiConnection_(uint8_t status);
    esp_err_t result = ESP_FAIL;

    private:
    esp_err_t Initialize_(void);
    esp_err_t ConnectWiFi_(void);
    esp_err_t RegisterWiFiEvents_(void);

    private:
    esp_netif_t* esp_netif_pointer_ = nullptr;
    TaskHandle_t process_handler_ = nullptr;
    credentials_st cred_area_;
    connection_st connection_area_;
    uint8_t connected_ = 0;
};

#endif /* NETWORK_MANAGER_GUARD */
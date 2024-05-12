#ifndef HTTP_SERVER_MANAGER_GUARD
#define HTTP_SERVER_MANAGER_GUARD

#include "HAL/memory/SharedMemoryManager.h"
#include "Managers/NetworkManager/inc/NetworkManager.h"
#include "Process/Template/ProcessTemplate.h"
#include "./RequestHandler/RequestHandler.h"

#include "esp_http_server.h"
#include "esp_err.h"

/**
 * A class that manages the HTTP connections.
 */
class HTTPServerManager : public ProcessTemplate {
   public:
    HTTPServerManager(const char* name, uint32_t memory, UBaseType_t priority)
        : ProcessTemplate(this, name, memory, priority, &this->_process_handler) {}

   public:
    SharedMemoryManager* memory_manager() const {
        return this->_shared_memory_manager;
    }

   private:
    void Execute(void);
    esp_err_t Initialize(void);
    esp_err_t StartHTTPServer(void);
    esp_err_t StopHTTPServer(void);
    esp_err_t RegisterHandlers(void);
    esp_err_t HandleAreaRequests(uint8_t area_index);
    void InitializeRequestList(void);

   private:
    static constexpr uint8_t maximum_requests_list_size = 8;

   public:
    char response_buffer[Request::MAXIMUM_REQUEST_REPLY];
    char read_buffer[Request::MAXIMUM_REQUEST_REPLY];

   private:
    httpd_uri_t _http_requests_list[maximum_requests_list_size];
    httpd_config_t _config;
    httpd_handle_t _server                      = NULL;
    TaskHandle_t _process_handler               = NULL;
    SharedMemoryManager* _shared_memory_manager = NULL;
    connection_st _connection_status{};
    connection_st _last_connection_status{};
    uint8_t _server_status = NetworkStatus::NOT_CONNECTED;
};

#endif /* HTTP_SERVER_MANAGER_GUARD */
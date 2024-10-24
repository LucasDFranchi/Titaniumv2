#ifndef HTTP_SERVER_MANAGER_H
#define HTTP_SERVER_MANAGER_H

#include "HAL/memory/SharedMemoryManager.h"
#include "SystemProcess/NetworkProcess/inc/NetworkProcess.h"
#include "SystemProcess/Template/ProcessTemplate.h"
#include "Protocols/Web/inc/RequestHandler.h"
#include "Protocols/Protobuf/inc/titanium.pb.h"
#include "esp_err.h"
#include "esp_http_server.h"

/**
 * @brief Class that manages the HTTP server functionality.
 *
 * This class handles the initialization, starting, stopping, and handling of HTTP server requests.
 */
class HTTPServerProcess : public ProcessTemplate {
   public:
    /**
     * @brief Constructor for HTTPServerProcess class.
     *
     * @param[in] name Name of the HTTP server manager process.
     * @param[in] memory Memory allocated for the process.
     * @param[in] priority Priority of the process.
     */
    HTTPServerProcess(const char* name, uint32_t memory, UBaseType_t priority)
        : ProcessTemplate(name, memory, priority, &this->_process_handler) {}

   public:
    /**
     * @brief Retrieves the shared memory manager instance.
     *
     * @return SharedMemoryManager* Pointer to the shared memory manager instance.
     */
    SharedMemoryManager* memory_manager() const {
        return this->_shared_memory_manager;
    }

   private:
    void Execute(void);
    titan_err_t Initialize(void);
    titan_err_t StartHTTPServer(void);
    titan_err_t StopHTTPServer(void);
    titan_err_t HandleAreaRequests(uint8_t area_index);
    titan_err_t InitializeRequestList(void);

   public:
    char response_buffer[Request::MAXIMUM_REQUEST_REPLY]; /**< Buffer for HTTP response data. */
    char read_buffer[Request::MAXIMUM_REQUEST_REPLY];     /**< Buffer for reading HTTP request data. */

   private:
    httpd_config_t _config;                                      /**< Configuration for HTTP server. */
    httpd_handle_t _server                      = nullptr;       /**< HTTP server handle. */
    TaskHandle_t _process_handler               = nullptr;       /**< Handle for the HTTP server process task. */
    SharedMemoryManager* _shared_memory_manager = nullptr;       /**< Pointer to shared memory manager instance. */
    network_information _connection_status{};                    /**< Current connection status. */
    network_information _last_connection_status{};               /**< Last recorded connection status. */
    uint8_t _server_status = NETWORK_STATUS_DISCONNECTED;        /**< Status of the HTTP server connection. */
};

#endif /* HTTP_SERVER_MANAGER_H */

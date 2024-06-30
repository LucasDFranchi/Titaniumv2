#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include "HAL/memory/MemoryHandlers.h"
#include "HAL/memory/SharedMemoryManager.h"

#include "esp_err.h"
#include "esp_http_server.h"

namespace Request {
    /**
     * @brief Maximum size of the request reply buffer.
     */
    constexpr uint16_t MAXIMUM_REQUEST_REPLY = 1024;
}  // namespace Request

uint32_t GetConnectionArea(char* buffer, SharedMemoryManager* shared_memory_manager);
uint32_t GetCredentialsArea(char* buffer, SharedMemoryManager* shared_memory_manager);
uint32_t GetCommunicationTransmitArea(char* buffer, SharedMemoryManager* shared_memory_manager);
esp_err_t PostConnectionArea(char* buffer, SharedMemoryManager* shared_memory_manager);
esp_err_t PostCredentialsArea(char* buffer, SharedMemoryManager* shared_memory_manager);
esp_err_t PostCommunicationTransmitArea(char* buffer, SharedMemoryManager* shared_memory_manager, uint8_t memory_area);
esp_err_t GetRequestKey(httpd_req_t* req, const char* buffer, uint16_t* key);
esp_err_t GetRequestData(httpd_req_t* req, char* buffer, uint16_t buffer_size);

#endif /* REQUEST_HANDLER_H */
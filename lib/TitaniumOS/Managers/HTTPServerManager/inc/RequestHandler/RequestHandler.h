#ifndef REQUEST_HANDLER_GUARD
#define REQUEST_HANDLER_GUARD

#include "HAL/memory/SharedMemoryManager.h"
#include "HAL/memory/MemoryHandlers.h"

#include "esp_http_server.h"
#include "esp_err.h"

namespace Request {
    constexpr uint16_t MAXIMUM_REQUEST_REPLY = 1024;
}

uint32_t GetConnectionArea(char* buffer, SharedMemoryManager* shared_memory_manager);
uint32_t GetCredentialsArea(char* buffer, SharedMemoryManager* shared_memory_manager);
uint32_t GetUartReceiveArea(char* buffer, SharedMemoryManager* shared_memory_manager);
uint32_t GetUartTransmitArea(char* buffer, SharedMemoryManager* shared_memory_manager);
uint32_t GetLoraReceiveArea(char* buffer, SharedMemoryManager* shared_memory_manager);
uint32_t GetLoraTransmitArea(char* buffer, SharedMemoryManager* shared_memory_manager);
esp_err_t PostConnectionArea(char* buffer, SharedMemoryManager* shared_memory_manager);
esp_err_t PostCredentialsArea(char* buffer, SharedMemoryManager* shared_memory_manager);
esp_err_t PostUartReceiveArea(char* buffer, SharedMemoryManager* shared_memory_manager);
esp_err_t PostUartTransmitArea(char* buffer, SharedMemoryManager* shared_memory_manager);
esp_err_t PostLoraReceiveArea(char* buffer, SharedMemoryManager* shared_memory_manager);
esp_err_t PostLoraTransmitArea(char* buffer, SharedMemoryManager* shared_memory_manager);
esp_err_t GetRequestKey(httpd_req_t* req, const char* buffer, uint16_t* key);

esp_err_t GetRequestData(httpd_req_t* req, char* buffer, uint16_t buffer_size);

#endif /* REQUEST_HANDLER_GUARD */
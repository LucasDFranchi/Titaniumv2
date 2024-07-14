#ifndef MQTT_CLIENT_PROCESS_H
#define MQTT_CLIENT_PROCESS_H

#include "HAL/memory/SharedMemoryManager.h"
#include "SystemProcess/NetworkProcess/inc/NetworkProcess.h"
#include "SystemProcess/Template/ProcessTemplate.h"

#include "esp_err.h"
#include "mqtt_client.h"

class MQTTClientProcess : public ProcessTemplate {
   public:
    /**
     * @brief Constructor for HTTPServerProcess class.
     *
     * @param[in] name Name of the HTTP server manager process.
     * @param[in] memory Memory allocated for the process.
     * @param[in] priority Priority of the process.
     */
    MQTTClientProcess(const char* name, uint32_t memory, UBaseType_t priority)
        : ProcessTemplate(name, memory, priority, &this->_process_handler) {}

   public:
    /**
     * @brief Retrieves the shared memory manager instance.
     *
     * @return SharedMemoryManager* Pointer to the shared memory manager instance.
     */
    SharedMemoryManager* memory_manager() const {
        return this->_shared_memory_manager.get();
    }

    esp_err_t PublishMemoryArea(uint8_t area_index);
    esp_err_t SubscribeMemoryArea(void);

   private:
    void Execute(void);
    esp_err_t Initialize(void);
    esp_err_t StartMQTTClient(void);
    esp_err_t StopMQTTClient(void);

   private:
    esp_mqtt_client_handle_t _client{};
    TaskHandle_t _process_handler                               = nullptr; /**< Handle for the HTTP server process task. */
    std::unique_ptr<SharedMemoryManager> _shared_memory_manager = nullptr;
    ConnectionStatusProtobuf _connection_status{};      /**< Current connection status. */
    ConnectionStatusProtobuf _last_connection_status{}; /**< Last recorded connection status. */
    MQTTClientProtobuf _mqtt_client_proto{};
    uint8_t _client_status = NetworkStatus::NOT_CONNECTED; /**< Status of the HTTP server connection. */
};

#endif /* MQTT_CLIENT_PROCESS_H */
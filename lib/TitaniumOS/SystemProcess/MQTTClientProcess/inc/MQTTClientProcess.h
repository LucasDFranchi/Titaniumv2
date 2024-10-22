#ifndef MQTT_CLIENT_PROCESS_H
#define MQTT_CLIENT_PROCESS_H

#include "Application/error/error_enum.h"
#include "HAL/memory/SharedMemoryManager.h"
#include "Protocols/Protobuf/inc/titanium.pb.h"
#include "SystemProcess/NetworkProcess/inc/NetworkProcess.h"
#include "SystemProcess/Template/ProcessTemplate.h"

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

    titan_err_t PublishMemoryArea(uint8_t area_index);
    titan_err_t SubscribeMemoryArea(void);

   private:
    void Execute(void);
    titan_err_t Initialize(void);
    titan_err_t StartMQTTClient(void);
    titan_err_t StopMQTTClient(void);
    titan_err_t PublishPackage(const char* topic, const char* raw_data);

   private:
    esp_mqtt_client_handle_t _client{};                                                        /**< MQTT client handle used for managing the connection to the MQTT broker. */
    broker_config _mqtt_client_proto{};                                                        /**< Configuration for the MQTT client, defining the broker address, port, and other settings. */
    station_status _sta_status{};                                                              /**< Current Station status, representing the connection status of the ESP32 in Station mode. */
    std::unique_ptr<SharedMemoryManager> _shared_memory_manager = nullptr;                     /**< Pointer to the Shared Memory Manager, responsible for managing shared data across processes. */
    TaskHandle_t _process_handler                               = nullptr;                     /**< Handle for the HTTP server process task, used to manage task execution and synchronization. */
    uint8_t _client_status                                      = NETWORK_STATUS_DISCONNECTED; /**< Status of the HTTP server connection, indicating whether the client is connected or disconnected. */
    bool _is_mqtt_started                                       = false;                       /**< Flag indicating whether the MQTT client has been successfully started. */
    bool _should_subscribe                                      = false;                       /**< Flag indicating whether the client should subscribe to MQTT topics upon connection. */
};

#endif /* MQTT_CLIENT_PROCESS_H */
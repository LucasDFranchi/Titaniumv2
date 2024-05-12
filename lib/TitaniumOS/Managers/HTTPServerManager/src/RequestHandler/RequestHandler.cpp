#include "./../../inc/RequestHandler/RequestHandler.h"

#include "HAL/memory/MemoryHandlers.h"
#include "Libraries/Json/inc/jsmn.h"
#include "Managers/ManagersAreaIndex.h"
#include "Managers/NetworkManager/inc/NetworkManagerDataModel.h"

#include "esp_log.h"

/* This should autogen based in schema */

namespace ConnectionJson {
    const std::string JSON_STRING("{\n\t\"connection_ap_status\": %d\n\t\"connection_sta_status\": %d\n}");
    const std::string CONNECTION_AP_STATUS_TOKEN_VALUE("connection_ap_status");
    const std::string CONNECTION_STA_STATUS_TOKEN_VALUE("connection_sta_status");
    const uint8_t CONNECTION_AP_STATUS_TOKEN  = 1;
    const uint8_t CONNECTION_STA_STATUS_TOKEN = 3;
    constexpr uint8_t NUM_TOKENS              = 5;
}  // namespace ConnectionJson

namespace CredentialsJson {
    const std::string JSON_STRING("{\n\t\"sta_ssid\": %s,\n\t\"sta_password\": %s\n}");
    const std::string STA_SSID_TOKEN_VALUE("sta_ssid");
    const std::string STA_PASSWORD_TOKEN_VALUE("sta_password");
    const uint8_t STA_SSID_TOKEN     = 1;
    const uint8_t STA_PASSWORD_TOKEN = 3;
    const uint8_t NUM_TOKENS         = 5;
}  // namespace CredentialsJson

namespace UartReceiveJson {
    const std::string JSON_STRING("{\n\t\"uart_buffer_rx\": %s\n}");
    const std::string UART_BUFFER_TOKEN_VALUE("uart_buffer_rx");
    const uint8_t UART_BUFFER_TOKEN = 1;
    const uint8_t NUM_TOKENS        = 3;
}  // namespace UartReceiveJson

namespace UartTransmitJson {
    const std::string JSON_STRING("{\n\t\"uart_buffer_tx\": %s\n}");
    const std::string UART_BUFFER_TOKEN_VALUE("uart_buffer_tx");
    const uint8_t UART_BUFFER_TOKEN = 1;
    const uint8_t NUM_TOKENS        = 3;
}  // namespace UartTransmitJson

namespace LoraReceiveJson {
    const std::string JSON_STRING("{\n\t\"lora_buffer_rx\": %s\n}");
    const std::string LORA_BUFFER_TOKEN_VALUE("lora_buffer_rx");
    const uint8_t LORA_BUFFER_TOKEN = 1;
    const uint8_t NUM_TOKENS        = 3;
}  // namespace LoraReceiveJson

namespace LoraTransmitJson {
    const std::string JSON_STRING("{\n\t\"lora_buffer_tx\": %s\n}");
    const std::string LORA_BUFFER_TOKEN_VALUE("lora_buffer_tx");
    const uint8_t LORA_BUFFER_TOKEN = 1;
    const uint8_t NUM_TOKENS        = 3;
}  // namespace LoraTransmitJson

/**
 * @brief Retrieves connection information from shared memory and formats it into a response buffer.
 *
 * Reads connection information from shared memory and formats it into a JSON-formatted string,
 * then copies the formatted string into the provided buffer.
 *
 * @param buffer Pointer to the buffer where the formatted connection JSON string will be stored.
 * @param shared_memory_manager Pointer to the SharedMemoryManager instance for reading from shared memory.
 *
 * @return Number of characters written to buffer if successful and within buffer size limit, otherwise 0.
 */
uint32_t GetConnectionArea(char* buffer, SharedMemoryManager* shared_memory_manager) {
    uint32_t response_length = 0;
    connection_st connection;

    do {
        if (buffer == nullptr) {
            break;
        }

        shared_memory_manager->Read(ManagersAreaIndex::CONNECTION, &connection);
        response_length = snprintf(buffer, Request::MAXIMUM_REQUEST_REPLY,
                                   ConnectionJson::JSON_STRING.c_str(),
                                   connection.connection_ap_status,
                                   connection.connection_sta_status);

        if (response_length > Request::MAXIMUM_REQUEST_REPLY) {
            response_length = 0;
        }

    } while (0);

    return response_length;
}

/**
 * @brief Retrieves credentials from shared memory and formats them into a response buffer.
 *
 * Reads credentials from shared memory and formats them into a JSON-formatted string,
 * then copies the formatted string into the provided buffer.
 *
 * @param buffer Pointer to the buffer where the formatted credentials JSON string will be stored.
 * @param shared_memory_manager Pointer to the SharedMemoryManager instance for reading from shared memory.
 *
 * @return Number of characters written to buffer if successful and within buffer size limit, otherwise 0.
 */
uint32_t GetCredentialsArea(char* buffer, SharedMemoryManager* shared_memory_manager) {
    uint32_t response_length = 0;
    credentials_st credentials;

    do {
        if (buffer == nullptr) {
            break;
        }

        shared_memory_manager->Read(ManagersAreaIndex::CREDENTIALS, &credentials);
        response_length = snprintf(buffer, Request::MAXIMUM_REQUEST_REPLY,
                                   CredentialsJson::JSON_STRING.c_str(),
                                   credentials.sta_ssid,
                                   credentials.sta_password);

        if (response_length > Request::MAXIMUM_REQUEST_REPLY) {
            response_length = 0;
        }

    } while (0);

    return response_length;
}

uint32_t GetUartReceiveArea(char* buffer, SharedMemoryManager* shared_memory_manager) {
    uint32_t response_length                   = 0;
    auto uart_memory_area_size                 = shared_memory_manager->GetAreaSize(ManagersAreaIndex::UART_RECEIVE);
    uint8_t uart_buffer[uart_memory_area_size] = {0};

    do {
        if (buffer == nullptr) {
            break;
        }

        shared_memory_manager->Read(ManagersAreaIndex::UART_RECEIVE, uart_buffer);
        response_length = snprintf(buffer, Request::MAXIMUM_REQUEST_REPLY,
                                   UartReceiveJson::JSON_STRING.c_str(),
                                   uart_buffer);

        if (response_length > Request::MAXIMUM_REQUEST_REPLY) {
            response_length = 0;
        }

    } while (0);

    return response_length;
}

uint32_t GetUartTransmitArea(char* buffer, SharedMemoryManager* shared_memory_manager) {
    uint32_t response_length                   = 0;
    auto uart_memory_area_size                 = shared_memory_manager->GetAreaSize(ManagersAreaIndex::UART_TRANSMIT);
    uint8_t uart_buffer[uart_memory_area_size] = {0};

    do {
        if (buffer == nullptr) {
            break;
        }

        shared_memory_manager->Read(ManagersAreaIndex::UART_TRANSMIT, uart_buffer);
        response_length = snprintf(buffer, Request::MAXIMUM_REQUEST_REPLY,
                                   UartTransmitJson::JSON_STRING.c_str(),
                                   uart_buffer);

        if (response_length > Request::MAXIMUM_REQUEST_REPLY) {
            response_length = 0;
        }

    } while (0);

    return response_length;
}

uint32_t GetLoraReceiveArea(char* buffer, SharedMemoryManager* shared_memory_manager) {
    uint32_t response_length                   = 0;
    auto lora_memory_area_size                 = shared_memory_manager->GetAreaSize(ManagersAreaIndex::LORA_RECEIVE);
    uint8_t lora_buffer[lora_memory_area_size] = {0};

    do {
        if (buffer == nullptr) {
            break;
        }

        shared_memory_manager->Read(ManagersAreaIndex::LORA_RECEIVE, lora_buffer);
        response_length = snprintf(buffer, Request::MAXIMUM_REQUEST_REPLY,
                                   LoraReceiveJson::JSON_STRING.c_str(),
                                   lora_buffer);

        if (response_length > Request::MAXIMUM_REQUEST_REPLY) {
            response_length = 0;
        }

    } while (0);

    return response_length;
}

uint32_t GetLoraTransmitArea(char* buffer, SharedMemoryManager* shared_memory_manager) {
    uint32_t response_length                   = 0;
    auto lora_memory_area_size                 = shared_memory_manager->GetAreaSize(ManagersAreaIndex::LORA_TRANSMIT);
    uint8_t lora_buffer[lora_memory_area_size] = {0};

    do {
        if (buffer == nullptr) {
            break;
        }

        shared_memory_manager->Read(ManagersAreaIndex::LORA_TRANSMIT, lora_buffer);
        response_length = snprintf(buffer, Request::MAXIMUM_REQUEST_REPLY,
                                   LoraTransmitJson::JSON_STRING.c_str(),
                                   lora_buffer);

        if (response_length > Request::MAXIMUM_REQUEST_REPLY) {
            response_length = 0;
        }

    } while (0);

    return response_length;
}

/**
 * @brief Writes connection information from a buffer to shared memory.
 *
 * Parses a JSON-formatted string in the buffer and writes the parsed connection
 * information to the shared memory manager.
 *
 * @param buffer Pointer to the buffer containing the JSON-formatted connection information.
 * @param shared_memory_manager Pointer to the SharedMemoryManager instance for writing to shared memory.
 *
 * @return ESP_OK if successfully parsed and written to shared memory, otherwise ESP_FAIL.
 */
esp_err_t PostConnectionArea(char* buffer, SharedMemoryManager* shared_memory_manager) {
    auto result = ESP_FAIL;
    connection_st connection{};
    jsmn_parser parser;
    jsmntok_t tokens[ConnectionJson::NUM_TOKENS];

    jsmn_init(&parser);

    do {
        if (buffer == nullptr) {
            break;
        }

        auto num_tokens = jsmn_parse(&parser, buffer, strlen(buffer), tokens, ConnectionJson::NUM_TOKENS);

        if (num_tokens != ConnectionJson::NUM_TOKENS) {
            break;
        }

        jsmntok_t key   = tokens[ConnectionJson::CONNECTION_AP_STATUS_TOKEN];
        jsmntok_t value = tokens[ConnectionJson::CONNECTION_AP_STATUS_TOKEN + 1];

        if (strncmp(buffer + key.start,
                    ConnectionJson::CONNECTION_AP_STATUS_TOKEN_VALUE.c_str(),
                    key.end - key.start) != 0) {
            break;
        }
        connection.connection_ap_status = atoi(buffer + value.start);

        key   = tokens[ConnectionJson::CONNECTION_STA_STATUS_TOKEN];
        value = tokens[ConnectionJson::CONNECTION_STA_STATUS_TOKEN + 1];

        if (strncmp(buffer + key.start,
                    ConnectionJson::CONNECTION_STA_STATUS_TOKEN_VALUE.c_str(),
                    key.end - key.start) != 0) {
            break;
        }
        connection.connection_sta_status = atoi(buffer + value.start);

        result = shared_memory_manager->Write(ManagersAreaIndex::CONNECTION,
                                              sizeof(connection_st),
                                              &connection);
    } while (0);

    return result;
}

/**
 * @brief Writes credentials from a buffer to shared memory.
 *
 * Parses a JSON-formatted string in the buffer and writes the parsed credentials
 * information to the shared memory manager.
 *
 * @param buffer Pointer to the buffer containing the JSON-formatted credentials information.
 * @param shared_memory_manager Pointer to the SharedMemoryManager instance for writing to shared memory.
 *
 * @return ESP_OK if successfully parsed and written to shared memory, otherwise ESP_FAIL.
 */
esp_err_t PostCredentialsArea(char* buffer, SharedMemoryManager* shared_memory_manager) {
    auto result = ESP_FAIL;
    credentials_st credentials{};
    jsmn_parser parser;
    jsmntok_t tokens[CredentialsJson::NUM_TOKENS];

    jsmn_init(&parser);

    do {
        if (buffer == nullptr) {
            break;
        }

        auto num_tokens = jsmn_parse(&parser, buffer, strlen(buffer), tokens, CredentialsJson::NUM_TOKENS);

        if (num_tokens != CredentialsJson::NUM_TOKENS) {
            break;
        }

        jsmntok_t key   = tokens[CredentialsJson::STA_SSID_TOKEN];
        jsmntok_t value = tokens[CredentialsJson::STA_SSID_TOKEN + 1];

        if (strncmp(buffer + key.start,
                    CredentialsJson::STA_SSID_TOKEN_VALUE.c_str(),
                    key.end - key.start) != 0) {
            break;
        }
        auto ssid_len = value.end - value.start;
        memcpy_s<uint8_t>(credentials.sta_ssid,
                          reinterpret_cast<uint8_t*>(buffer + value.start),
                          ssid_len);

        key   = tokens[CredentialsJson::STA_PASSWORD_TOKEN];
        value = tokens[CredentialsJson::STA_PASSWORD_TOKEN + 1];

        if (strncmp(buffer + key.start,
                    CredentialsJson::STA_PASSWORD_TOKEN_VALUE.c_str(),
                    key.end - key.start) != 0) {
            break;
        }
        auto password_len = value.end - value.start;
        memcpy_s<uint8_t>(credentials.sta_password,
                          reinterpret_cast<uint8_t*>(buffer + value.start),
                          password_len);

        result = shared_memory_manager->Write(ManagersAreaIndex::CREDENTIALS,
                                              sizeof(credentials_st),
                                              &credentials);
    } while (0);

    return result;
}

esp_err_t PostUartReceiveArea(char* buffer, SharedMemoryManager* shared_memory_manager) {
    auto result                                = ESP_FAIL;
    auto uart_memory_area_size                 = shared_memory_manager->GetAreaSize(ManagersAreaIndex::UART_RECEIVE);
    uint8_t uart_buffer[uart_memory_area_size] = {0};
    jsmn_parser parser;
    jsmntok_t tokens[UartReceiveJson::NUM_TOKENS];

    jsmn_init(&parser);

    do {
        if (buffer == nullptr) {
            break;
        }

        auto num_tokens = jsmn_parse(&parser, buffer, strlen(buffer), tokens, UartReceiveJson::NUM_TOKENS);

        if (num_tokens != UartReceiveJson::NUM_TOKENS) {
            break;
        }

        jsmntok_t key   = tokens[UartReceiveJson::UART_BUFFER_TOKEN];
        jsmntok_t value = tokens[UartReceiveJson::UART_BUFFER_TOKEN + 1];

        if (strncmp(buffer + key.start,
                    UartReceiveJson::UART_BUFFER_TOKEN_VALUE.c_str(),
                    key.end - key.start) != 0) {
            break;
        }
        auto uart_buffer_len = value.end - value.start;
        memcpy_s<uint8_t>(uart_buffer,
                          reinterpret_cast<uint8_t*>(buffer + value.start),
                          uart_buffer_len);

        result = shared_memory_manager->Write(ManagersAreaIndex::UART_RECEIVE,
                                              uart_memory_area_size,
                                              uart_buffer);
    } while (0);

    return result;
}

esp_err_t PostUartTransmitArea(char* buffer, SharedMemoryManager* shared_memory_manager) {
    auto result                                = ESP_FAIL;
    auto uart_memory_area_size                 = shared_memory_manager->GetAreaSize(ManagersAreaIndex::UART_TRANSMIT);
    uint8_t uart_buffer[uart_memory_area_size] = {0};
    jsmn_parser parser;
    jsmntok_t tokens[UartTransmitJson::NUM_TOKENS];

    jsmn_init(&parser);

    do {
        if (buffer == nullptr) {
            break;
        }

        auto num_tokens = jsmn_parse(&parser, buffer, strlen(buffer), tokens, UartTransmitJson::NUM_TOKENS);

        if (num_tokens != UartTransmitJson::NUM_TOKENS) {
            break;
        }

        jsmntok_t key   = tokens[UartTransmitJson::UART_BUFFER_TOKEN];
        jsmntok_t value = tokens[UartTransmitJson::UART_BUFFER_TOKEN + 1];

        if (strncmp(buffer + key.start,
                    UartTransmitJson::UART_BUFFER_TOKEN_VALUE.c_str(),
                    key.end - key.start) != 0) {
            break;
        }
        auto uart_buffer_len = value.end - value.start;
        memcpy_s<uint8_t>(uart_buffer,
                          reinterpret_cast<uint8_t*>(buffer + value.start),
                          uart_buffer_len);

        result = shared_memory_manager->Write(ManagersAreaIndex::UART_TRANSMIT,
                                              uart_memory_area_size,
                                              uart_buffer);
    } while (0);

    return result;
}

esp_err_t PostLoraReceiveArea(char* buffer, SharedMemoryManager* shared_memory_manager) {
    auto result                                = ESP_FAIL;
    auto lora_memory_area_size                 = shared_memory_manager->GetAreaSize(ManagersAreaIndex::LORA_RECEIVE);
    uint8_t lora_buffer[lora_memory_area_size] = {0};
    jsmn_parser parser;
    jsmntok_t tokens[LoraReceiveJson::NUM_TOKENS];

    jsmn_init(&parser);

    do {
        if (buffer == nullptr) {
            break;
        }

        auto num_tokens = jsmn_parse(&parser, buffer, strlen(buffer), tokens, LoraReceiveJson::NUM_TOKENS);

        if (num_tokens != LoraReceiveJson::NUM_TOKENS) {
            break;
        }

        jsmntok_t key   = tokens[LoraReceiveJson::LORA_BUFFER_TOKEN];
        jsmntok_t value = tokens[LoraReceiveJson::LORA_BUFFER_TOKEN + 1];

        if (strncmp(buffer + key.start,
                    LoraReceiveJson::LORA_BUFFER_TOKEN_VALUE.c_str(),
                    key.end - key.start) != 0) {
            break;
        }
        auto lora_buffer_len = value.end - value.start;
        memcpy_s<uint8_t>(lora_buffer,
                          reinterpret_cast<uint8_t*>(buffer + value.start),
                          lora_buffer_len);

        result = shared_memory_manager->Write(ManagersAreaIndex::LORA_RECEIVE,
                                              lora_memory_area_size,
                                              lora_buffer);
    } while (0);

    return result;
}

esp_err_t PostLoraTransmitArea(char* buffer, SharedMemoryManager* shared_memory_manager) {
    auto result                                = ESP_FAIL;
    auto lora_memory_area_size                 = shared_memory_manager->GetAreaSize(ManagersAreaIndex::LORA_TRANSMIT);
    uint8_t lora_buffer[lora_memory_area_size] = {0};
    jsmn_parser parser;
    jsmntok_t tokens[LoraTransmitJson::NUM_TOKENS];

    jsmn_init(&parser);

    do {
        if (buffer == nullptr) {
            break;
        }

        auto num_tokens = jsmn_parse(&parser, buffer, strlen(buffer), tokens, LoraTransmitJson::NUM_TOKENS);

        if (num_tokens != LoraTransmitJson::NUM_TOKENS) {
            break;
        }

        jsmntok_t key   = tokens[LoraTransmitJson::LORA_BUFFER_TOKEN];
        jsmntok_t value = tokens[LoraTransmitJson::LORA_BUFFER_TOKEN + 1];

        if (strncmp(buffer + key.start,
                    LoraTransmitJson::LORA_BUFFER_TOKEN_VALUE.c_str(),
                    key.end - key.start) != 0) {
            break;
        }
        auto lora_buffer_len = value.end - value.start;
        memcpy_s<uint8_t>(lora_buffer,
                          reinterpret_cast<uint8_t*>(buffer + value.start),
                          lora_buffer_len);

        result = shared_memory_manager->Write(ManagersAreaIndex::LORA_TRANSMIT,
                                              lora_memory_area_size,
                                              lora_buffer);
    } while (0);

    return result;
}

esp_err_t GetRequestKey(httpd_req_t* req, const char* buffer, uint16_t* key) {
    esp_err_t result                     = ESP_OK;
    constexpr uint8_t MAXIMUM_KEY_SIZE   = 3;
    char array_key[MAXIMUM_KEY_SIZE + 1] = {0};

    uint8_t key_string_start = strlen(buffer);
    uint8_t key_string_size  = strlen(req->uri) - key_string_start;

    do {
        if (key_string_size > MAXIMUM_KEY_SIZE) {
            result = ESP_FAIL;
            break;
        }

        if (memcpy_s<char>(array_key, (char*)&req->uri[key_string_start],
                           key_string_size) != ESP_OK) {
            result = ESP_FAIL;
            break;
        }

        array_key[MAXIMUM_KEY_SIZE] = '\0';
        *key                        = atoi(array_key);

    } while (0);

    return result;
}

esp_err_t GetRequestData(httpd_req_t* req, char* buffer, uint16_t buffer_size) {
    esp_err_t result = ESP_OK;

    do {
        if (req->content_len > Request::MAXIMUM_REQUEST_REPLY) {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                "Request Size is too big");
            result = ESP_FAIL;
            break;
        }

        memset_s<char>(buffer, 0, buffer_size);

        if (httpd_req_recv(req, buffer, req->content_len) != req->content_len) {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Received Fail");
            result = ESP_FAIL;
            break;
        }

    } while (0);

    return result;
}
#include "./../../inc/RequestHandler/RequestHandler.h"

#include "HAL/memory/MemoryHandlers.h"
#include "Libraries/Json/inc/jsmn.h"
#include "SystemProcess/CommunicationProcess/inc/CommunicationProcessDataModel.h"
#include "SystemProcess/NetworkProcess/inc/NetworkProcessDataModel.h"
#include "SystemProcess/ProcessAreasIndex.h"

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

namespace CommunicationTransmitJson {
    const std::string JSON_STRING("{\n\t\"memory_area\": %d,\n\t\"command\": %d\n}");
    const std::string MEMORY_AREA_TOKEN_VALUE("memory_area");
    const std::string COMMAND_TOKEN_VALUE("command");
    const uint8_t MEMORY_AREA_TOKEN = 1;
    const uint8_t COMMAND_TOKEN     = 3;
    const uint8_t NUM_TOKENS        = 5;
}  // namespace CommunicationTransmitJson

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

        shared_memory_manager->Read(ProcessAreaIndex::CONNECTION, &connection);
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

        shared_memory_manager->Read(ProcessAreaIndex::CREDENTIALS, &credentials);
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

uint32_t GetCommunicationTransmitArea(char* buffer, SharedMemoryManager* shared_memory_manager) {
    uint32_t response_length = 0;
    communication_request_st communication_request;

    do {
        if (buffer == nullptr) {
            break;
        }

        shared_memory_manager->Read(ProcessAreaIndex::UART_TRANSMIT, &communication_request);
        response_length = snprintf(buffer, Request::MAXIMUM_REQUEST_REPLY,
                                   CommunicationTransmitJson::JSON_STRING.c_str(),
                                   communication_request.memory_area,
                                   communication_request.command);

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

        result = shared_memory_manager->Write(ProcessAreaIndex::CONNECTION,
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

        result = shared_memory_manager->Write(ProcessAreaIndex::CREDENTIALS,
                                              sizeof(credentials_st),
                                              &credentials);
    } while (0);

    return result;
}

esp_err_t PostCommunicationTransmitArea(char* buffer, SharedMemoryManager* shared_memory_manager, uint8_t memory_area) {
    auto result = ESP_FAIL;
    communication_request_st communication_request{};
    jsmn_parser parser;
    jsmntok_t tokens[CommunicationTransmitJson::NUM_TOKENS];

    jsmn_init(&parser);

    do {
        if (buffer == nullptr) {
            break;
        }

        auto num_tokens = jsmn_parse(&parser, buffer, strlen(buffer), tokens, CommunicationTransmitJson::NUM_TOKENS);

        if (num_tokens != CommunicationTransmitJson::NUM_TOKENS) {
            break;
        }

        jsmntok_t key   = tokens[CommunicationTransmitJson::MEMORY_AREA_TOKEN];
        jsmntok_t value = tokens[CommunicationTransmitJson::MEMORY_AREA_TOKEN + 1];

        if (strncmp(buffer + key.start,
                    CommunicationTransmitJson::MEMORY_AREA_TOKEN_VALUE.c_str(),
                    key.end - key.start) != 0) {
            break;
        }

        communication_request.memory_area = atoi(buffer + value.start);

        key   = tokens[CommunicationTransmitJson::COMMAND_TOKEN];
        value = tokens[CommunicationTransmitJson::COMMAND_TOKEN + 1];

        if (strncmp(buffer + key.start,
                    CommunicationTransmitJson::COMMAND_TOKEN_VALUE.c_str(),
                    key.end - key.start) != 0) {
            break;
        }

        communication_request.command = static_cast<command_e>(atoi(buffer + value.start));

        result = shared_memory_manager->Write(memory_area,
                                              sizeof(communication_request_st),
                                              &communication_request);
    } while (0);

    return result;
}


/**
 * @brief Extracts a numeric key from the URI of an HTTP request.
 *
 * Extracts a numeric key from the URI of the provided HTTP request. The key is located
 * at the end of the URI string after the 'buffer' parameter string.
 *
 * @param[in] req Pointer to the HTTP request structure from which the URI is extracted.
 * @param[in] buffer Pointer to a null-terminated string that specifies the start of the key in the URI.
 * @param[out] key Pointer to a variable where the extracted key will be stored.
 *
 * @return ESP_OK if the key extraction is successful and within size limits, otherwise ESP_FAIL.
 */
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

/**
 * @brief Receives and stores the data payload from an HTTP request into a buffer.
 *
 * Receives and stores the data payload from the provided HTTP request into the specified buffer.
 * Checks if the received data size exceeds the maximum allowed size and sends an error response
 * if it does.
 *
 * @param[in] req Pointer to the HTTP request structure from which data is received.
 * @param[out] buffer Pointer to the buffer where received data will be stored.
 * @param[in] buffer_size Size of the buffer allocated for storing received data.
 *
 * @return ESP_OK if data reception is successful and within size limits, otherwise ESP_FAIL.
 */
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
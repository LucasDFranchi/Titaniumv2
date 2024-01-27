#include "RequestHandler.hpp"

#include "MemoryUtils.h"
#include "PrintUtils.h"

uint32_t GetConnectionArea(char* buffer, MemoryManager* memory_manager) {
    uint32_t response_length = 0;
    const char* response =
        "{\n\t\"connection_ap_status\": %d\n\t\"connection_sta_status\": %d\n}";

    connection_st connection;
    memory_manager->Read(CONNECTION_AREA, &connection);
    response_length = snprintf_s(buffer, REQUEST::MAXIMUM_REQUEST_REPLY,
                                 response, connection.connection_ap_status,
                                 connection.connection_sta_status);

    return response_length <= REQUEST::MAXIMUM_REQUEST_REPLY ? response_length
                                                             : 0;
}

uint32_t GetCredentialsArea(char* buffer, MemoryManager* memory_manager) {
    uint32_t response_length = 0;
    const char* response = "{\n\t\"sta_ssid\": %s,\n\t\"sta_password\": %s\n}";

    credentials_st credentials;
    memory_manager->Read(CREDENTIALS_AREA, &credentials);
    response_length =
        snprintf_s(buffer, REQUEST::MAXIMUM_REQUEST_REPLY, response,
                   credentials.sta_ssid, credentials.sta_password);

    return response_length <= REQUEST::MAXIMUM_REQUEST_REPLY ? response_length
                                                             : 0;
}

uint32_t GetLoraReadArea(char* buffer, MemoryManager* memory_manager) {
    uint32_t response_length = 0;
    char array_string[256] = {0};
    const char* response = "{\n\t\"rx_buffer\": %s\n}";

    lora_read_st lora_read;
    memory_manager->Read(LORA_READ_AREA, &lora_read);

    snprintf_array(array_string, lora_read.rx_buffer,
                   sizeof(lora_read.rx_buffer), sizeof(array_string));

    response_length = snprintf_s(buffer, REQUEST::MAXIMUM_REQUEST_REPLY,
                                 response, array_string);

    return response_length <= REQUEST::MAXIMUM_REQUEST_REPLY ? response_length
                                                             : 0;
}

uint32_t GetLoraWriteArea(char* buffer, MemoryManager* memory_manager) {
    uint32_t response_length = 0;
    char array_string[256] = {0};
    const char* response = "{\n\t\"tx_buffer\": %s\n}";

    lora_write_st lora_write;
    memory_manager->Read(LORA_WRITE_AREA, &lora_write);

    snprintf_array(array_string, lora_write.tx_buffer,
                   sizeof(lora_write.tx_buffer), sizeof(array_string));

    response_length = snprintf_s(buffer, REQUEST::MAXIMUM_REQUEST_REPLY,
                                 response, array_string);

    return response_length <= REQUEST::MAXIMUM_REQUEST_REPLY ? response_length
                                                             : 0;
}

esp_err_t GetRequestKey(httpd_req_t* req, const char* buffer, uint16_t* key) {
    esp_err_t result = ESP_OK;
    constexpr uint8_t MAXIMUM_KEY_SIZE = 3;
    char array_key[MAXIMUM_KEY_SIZE + 1] = {0};

    uint8_t key_string_start = strlen(buffer);
    uint8_t key_string_size = strlen(req->uri) - key_string_start;

    do {
        if (key_string_size > MAXIMUM_KEY_SIZE) {
            result = ESP_FAIL;
            break;
        }

        if (memcpy_s(array_key, (char*)&req->uri[key_string_start],
                     key_string_size) != ESP_OK) {
            result = ESP_FAIL;
            break;
        }

        array_key[MAXIMUM_KEY_SIZE] = '\0';
        *key = atoi(array_key);

    } while (0);

    return result;
}

esp_err_t GetRequestData(httpd_req_t* req, char* buffer, uint16_t buffer_size) {
    esp_err_t result = ESP_OK;

    do {
        if (req->content_len > REQUEST::MAXIMUM_REQUEST_REPLY) {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                "Request Size is too big");
            result = ESP_FAIL;
            break;
        }

        memset_s(buffer, 0, sizeof(buffer_size));

        if (httpd_req_recv(req, buffer, req->content_len) != req->content_len) {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Received Fail");
            result = ESP_FAIL;
            break;
        }

    } while (0);

    return result;
}
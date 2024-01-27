#include "Protocols/Web/inc/RequestHandler.h"

#include "HAL/memory/MemoryHandlers.h"

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
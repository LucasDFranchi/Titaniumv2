/**
 * @file HTTPServerProcess.cpp
 * @brief Implementation of HTTPServerProcess class methods and HTTP request handlers.
 */

#include "./../inc/HTTPServerProcess.h"
#include "./../inc/RequestHandler/RequestHandler.h"

#include "esp_log.h"

#include "HAL/memory/MemoryHandlers.h"
#include "SystemProcess/ProcessAreasIndex.h"

static const char* TAG = "HTTPServerProcess"; /**< Logging tag for HTTPServerProcess class. */

namespace BinaryData {
    extern const uint8_t index_html_start[] asm("_binary_index_html_start");          /**< Start of index.html binary data. */
    extern const uint8_t index_html_end[] asm("_binary_index_html_end");              /**< End of index.html binary data. */
    extern const uint8_t styles_css_start[] asm("_binary_styles_css_start");          /**< Start of styles.css binary data. */
    extern const uint8_t styles_css_end[] asm("_binary_styles_css_end");              /**< End of styles.css binary data. */
    extern const uint8_t app_js_start[] asm("_binary_app_js_start");                  /**< Start of app.js binary data. */
    extern const uint8_t app_js_end[] asm("_binary_app_js_end");                      /**< End of app.js binary data. */
    extern const uint8_t jquery3_js_start[] asm("_binary_jquery_3_3_1_min_js_start"); /**< Start of jquery-3.3.1.min.js binary data. */
    extern const uint8_t jquery3_js_end[] asm("_binary_jquery_3_3_1_min_js_end");     /**< End of jquery-3.3.1.min.js binary data. */
    extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start");        /**< Start of favicon.ico binary data. */
    extern const uint8_t favicon_ico_end[] asm("_binary_favicon_ico_end");            /**< End of favicon.ico binary data. */
}  // namespace BinaryData

/**
 * @brief HTTP GET handler for serving index.html.
 *
 * @param[in] req HTTP request object.
 * @return ESP_OK on success, or an error code on failure.
 */
static esp_err_t get_uri_index_html(httpd_req_t* req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req,
                    reinterpret_cast<const char*>(BinaryData::index_html_start),
                    BinaryData::index_html_end - BinaryData::index_html_start);

    return ESP_OK;
}

/**
 * @brief HTTP GET handler for serving styles.css.
 *
 * @param[in] req HTTP request object.
 * @return ESP_OK on success, or an error code on failure.
 */
static esp_err_t get_uri_get_app_css(httpd_req_t* req) {
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req,
                    reinterpret_cast<const char*>(BinaryData::styles_css_start),
                    BinaryData::styles_css_end - BinaryData::styles_css_start);

    return ESP_OK;
}

/**
 * @brief HTTP GET handler for serving app.js.
 *
 * @param[in] req HTTP request object.
 * @return ESP_OK on success, or an error code on failure.
 */
static esp_err_t get_uri_get_app_js(httpd_req_t* req) {
    httpd_resp_set_type(req, "text/javascript");
    httpd_resp_send(req,
                    reinterpret_cast<const char*>(BinaryData::app_js_start),
                    BinaryData::app_js_end - BinaryData::app_js_start - 1);

    return ESP_OK;
}

/**
 * @brief HTTP GET handler for serving jquery-3.3.1.min.js.
 *
 * @param[in] req HTTP request object.
 * @return ESP_OK on success, or an error code on failure.
 */
static esp_err_t get_uri_get_jquery_js(httpd_req_t* req) {
    httpd_resp_set_type(req, "text/javascript");
    httpd_resp_send(
        req, reinterpret_cast<const char*>(BinaryData::jquery3_js_start),
        BinaryData::jquery3_js_end - BinaryData::jquery3_js_start - 1);

    return ESP_OK;
}

/**
 * @brief HTTP GET handler for serving favicon.ico.
 *
 * @param[in] req HTTP request object.
 * @return ESP_OK on success, or an error code on failure.
 */
static esp_err_t get_uri_favicon_icon(httpd_req_t* req) {
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(
        req, reinterpret_cast<const char*>(BinaryData::favicon_ico_start),
        BinaryData::favicon_ico_end - BinaryData::favicon_ico_start);

    return ESP_OK;
}

/**
 * @brief HTTP POST handler for processing WiFi credentials.
 *
 * @param[in] req HTTP request object.
 * @return ESP_OK on success, or an error code on failure.
 */
static esp_err_t post_uri_wifi_credentials(httpd_req_t* req) {
    CredentialsProtobuf credentials_proto;
    char ssid[32]     = {0};
    char password[64] = {0};
    esp_err_t result  = ESP_FAIL;

    HTTPServerProcess* http_server_manager =
        reinterpret_cast<HTTPServerProcess*>(req->user_ctx);

    uint8_t ssid_size =
        httpd_req_get_hdr_value_len(req, "my-connected-ssid") + 1;
    uint8_t pwd_size = httpd_req_get_hdr_value_len(req, "my-connected-pwd") + 1;

    do {
        if (ssid_size <= 0) {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                "SSID size equals to 0!");
            return ESP_FAIL;
        }

        if (pwd_size <= 0) {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                "Password size equals to 0!");
            return ESP_FAIL;
        }

        if (httpd_req_get_hdr_value_str(req, "my-connected-ssid", ssid,
                                        ssid_size) != ESP_OK) {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                "Error reading SSID!");
            return ESP_FAIL;
        }

        if (httpd_req_get_hdr_value_str(req, "my-connected-pwd", password,
                                        pwd_size) != ESP_OK) {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                "Error reading Password!");
            return ESP_FAIL;
        }
        credentials_proto.UpdateSsid(ssid);
        credentials_proto.UpdatePassword(password);

        http_server_manager->memory_manager()->Write(ProcessAreaIndex::CREDENTIALS, &credentials_proto);
        result = ESP_OK;

    } while (0);

    return result;
}

/**
 * @brief HTTP GET/POST handler for retrieving/posting data from/to different memory areas.
 *
 * @param[in] req HTTP request object.
 * @return ESP_OK on success, or an error code on failure.
 */
static esp_err_t get_area_handler(httpd_req_t* req) {
    uint32_t response_size = 0;
    uint16_t area_index    = 0;

    if (GetRequestKey(req, "/get_area?id=", &area_index)) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Index out of range!");
        return ESP_FAIL;
    }

    HTTPServerProcess* http_server_manager =
        reinterpret_cast<HTTPServerProcess*>(req->user_ctx);

    if (req->method == HTTP_GET) {
        switch (area_index) {
            case ProcessAreaIndex::CONNECTION: {
                response_size =
                    GetConnectionArea(&http_server_manager->response_buffer[0],
                                      http_server_manager->memory_manager());
            } break;
            case ProcessAreaIndex::CREDENTIALS: {
                response_size =
                    GetCredentialsArea(&http_server_manager->response_buffer[0],
                                       http_server_manager->memory_manager());
            } break;
            case ProcessAreaIndex::UART_TRANSMIT:
            case ProcessAreaIndex::LORA_TRANSMIT: {
                response_size =
                    GetCommunicationTransmitArea(&http_server_manager->response_buffer[0],
                                                 http_server_manager->memory_manager());
            } break;
            case CustomProcessAreaIndex::WATER_LEVEL: {
                response_size =
                    GetWaterLevelArea(&http_server_manager->response_buffer[0],
                                                 http_server_manager->memory_manager());
            } break;
            default: {
                httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                    "Invalid Memory Area!");
                return ESP_FAIL;
            }
        }
        if (response_size > 0) {
            httpd_resp_send(req, http_server_manager->response_buffer,
                            response_size);
        }
    } else if ((req->method == HTTP_POST)) {
        esp_err_t result = ESP_FAIL;

        if (GetRequestData(req, http_server_manager->read_buffer,
                           sizeof(http_server_manager->read_buffer)) != ESP_OK) {
            return ESP_FAIL;
        }

        switch (area_index) {
            case ProcessAreaIndex::CONNECTION: {
                result = PostConnectionArea(http_server_manager->read_buffer,
                                            http_server_manager->memory_manager());
            } break;
            case ProcessAreaIndex::CREDENTIALS: {
                result = PostCredentialsArea(http_server_manager->read_buffer,
                                             http_server_manager->memory_manager());
            } break;
            case ProcessAreaIndex::UART_TRANSMIT:
            case ProcessAreaIndex::LORA_TRANSMIT: {
                result = PostCommunicationTransmitArea(http_server_manager->read_buffer,
                                                       http_server_manager->memory_manager(),
                                                       area_index);
            } break;
            default: {
                httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                    "Invalid Memory Area!");
                return ESP_FAIL;
            }
        }

        if (result == ESP_OK) {
            httpd_resp_sendstr_chunk(req, "OK");
        } else {
            httpd_resp_sendstr_chunk(req, "NOK");
        }

        httpd_resp_sendstr_chunk(req, NULL);
    }

    return ESP_OK;
}

/**
 * @brief Main execution loop for HTTPServerProcess.
 *
 * This function runs an infinite loop with a delay of 1000 milliseconds.
 */
void HTTPServerProcess::Execute(void) {
    this->Initialize();

    while (1) {
        if (this->_shared_memory_manager->IsAreaDataUpdated(ProcessAreaIndex::CONNECTION)) {
            this->_shared_memory_manager->Read(ProcessAreaIndex::CONNECTION,
                                               &this->_connection_status);
        }

        do {
            auto ap_changed =
                this->_last_connection_status.GetApStatus() !=
                this->_connection_status.GetApStatus();
            auto sta_changed =
                this->_last_connection_status.GetStaStatus() !=
                this->_connection_status.GetStaStatus();

            if (!ap_changed && !sta_changed) {
                break;
            }

            auto ap_status  = this->_connection_status.GetApStatus();
            auto sta_status = this->_connection_status.GetStaStatus();

            if ((ap_status == NetworkStatus::CONNECTED) || (sta_status == NetworkStatus::CONNECTED)) {
                if (this->_server_status != NetworkStatus::CONNECTED) {
                    this->StartHTTPServer();
                    this->_server_status = NetworkStatus::CONNECTED;
                    ESP_LOGI(TAG, "HTTP SERVER STARTED");
                }
            } else if ((ap_status == NetworkStatus::NOT_CONNECTED) && (sta_status == NetworkStatus::NOT_CONNECTED)) {
                if (this->_server_status != NetworkStatus::CONNECTED) {
                    this->StopHTTPServer();
                    this->_server_status = NetworkStatus::NOT_CONNECTED;
                    ESP_LOGI(TAG, "HTTP SERVER STOPED");
                }
            }
        } while (0);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Initializes the HTTPManager by configuring server parameters and initializing request handlers.
 *
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t HTTPServerProcess::Initialize(void) {
    auto result = ESP_OK;

    this->_config                   = HTTPD_DEFAULT_CONFIG();
    this->_config.send_wait_timeout = 10;
    this->_config.recv_wait_timeout = 10;
    this->_config.max_uri_handlers  = 20;
    this->_shared_memory_manager    = SharedMemoryManager::GetInstance();

    this->InitializeRequestList();

    return result;
}

/**
 * @brief Starts the HTTP server and registers URI handlers.
 *
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t HTTPServerProcess::StartHTTPServer(void) {
    auto result = ESP_OK;

    if (httpd_start(&this->_server, &this->_config) == ESP_OK) {
        result += RegisterHandlers();
    }

    return result;
}

/**
 * @brief Stops the HTTP server.
 *
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t HTTPServerProcess::StopHTTPServer(void) {
    return httpd_stop(&this->_server);
}

/**
 * @brief Initializes the list of HTTP request URIs and their corresponding handlers.
 */
void HTTPServerProcess::InitializeRequestList(void) {
    static const httpd_uri_t uri_get_area_get = {
        .uri      = "/get_area",
        .method   = HTTP_GET,
        .handler  = get_area_handler,
        .user_ctx = this,
    };

    static const httpd_uri_t uri_get_area_post = {
        .uri      = "/get_area",
        .method   = HTTP_POST,
        .handler  = get_area_handler,
        .user_ctx = this,
    };

    static const httpd_uri_t uri_index_html = {
        .uri      = "/",
        .method   = HTTP_GET,
        .handler  = get_uri_index_html,
        .user_ctx = this,
    };

    static const httpd_uri_t uri_get_styles_css = {
        .uri      = "/styles.css",
        .method   = HTTP_GET,
        .handler  = get_uri_get_app_css,
        .user_ctx = this,
    };

    static const httpd_uri_t uri_get_app_js = {
        .uri      = "/app.js",
        .method   = HTTP_GET,
        .handler  = get_uri_get_app_js,
        .user_ctx = this,
    };

    static const httpd_uri_t uri_get_jquery_js = {
        .uri      = "/jquery-3.3.1.min.js",
        .method   = HTTP_GET,
        .handler  = get_uri_get_jquery_js,
        .user_ctx = this,
    };

    static const httpd_uri_t uri_get_favicon_ico = {
        .uri      = "/favicon.ico",
        .method   = HTTP_GET,
        .handler  = get_uri_favicon_icon,
        .user_ctx = this,
    };

    static const httpd_uri_t uri_post_credentials = {
        .uri      = "/wifiCredentials.json",
        .method   = HTTP_POST,
        .handler  = post_uri_wifi_credentials,
        .user_ctx = this,
    };

    this->_http_requests_list[0] = uri_get_area_get;
    this->_http_requests_list[1] = uri_get_area_post;
    this->_http_requests_list[2] = uri_index_html;
    this->_http_requests_list[3] = uri_get_styles_css;
    this->_http_requests_list[4] = uri_get_app_js;
    this->_http_requests_list[5] = uri_get_jquery_js;
    this->_http_requests_list[6] = uri_get_favicon_ico;
    this->_http_requests_list[7] = uri_post_credentials;
}

/**
 * @brief Registers HTTP request handlers with the HTTP server.
 *
 * @return ESP_OK on success, or an error code on failure.
 */
esp_err_t HTTPServerProcess::RegisterHandlers(void) {
    auto result = ESP_OK;

    for (int i = 0; i < this->maximum_requests_list_size; i++) {
        result += httpd_register_uri_handler(this->_server,
                                             &this->_http_requests_list[i]);
    }

    return result;
}

/**
 * @file HTTPServerProcess.cpp
 * @brief Implementation of HTTPServerProcess class methods and HTTP request handlers.
 */

#include "SystemProcess/HTTPServerProcess/inc/HTTPServerProcess.h"
#include "HAL/memory/MemoryHandlers.h"

#include "esp_log.h"

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
    extern const uint8_t titanium_proto_start[] asm("_binary_titanium_proto_start");  /**< Start of titanium.proto binary data. */
    extern const uint8_t titanium_proto_end[] asm("_binary_titanium_proto_end");      /**< End of titanium.proto binary data. */
}  // namespace BinaryData

/**
 * @brief HTTP GET handler for serving index.html.
 *
 * @param[in] req HTTP request object.
 * @return ESP_OK on success, or an error code on failure.
 */
static titan_err_t get_uri_index_html(httpd_req_t* req) {
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
static titan_err_t get_uri_get_app_css(httpd_req_t* req) {
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
static titan_err_t get_uri_get_app_js(httpd_req_t* req) {
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
static titan_err_t get_uri_get_jquery_js(httpd_req_t* req) {
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
static titan_err_t get_uri_favicon_icon(httpd_req_t* req) {
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(
        req, reinterpret_cast<const char*>(BinaryData::favicon_ico_start),
        BinaryData::favicon_ico_end - BinaryData::favicon_ico_start);

    return ESP_OK;
}

/**
 * @brief HTTP GET handler for serving a .proto file.
 *
 * @param[in] req HTTP request object.
 * @return ESP_OK on success, or an error code on failure.
 */
static titan_err_t get_uri_proto_file(httpd_req_t* req) {
    // Set the response content type to text/protobuf
    httpd_resp_set_type(req, "text/protobuf");
    
    // Send the .proto file content
    httpd_resp_send(
        req, reinterpret_cast<const char*>(BinaryData::titanium_proto_start),
        BinaryData::titanium_proto_end - BinaryData::titanium_proto_start);
    
    return ESP_OK;
}

/**
 * @brief HTTP POST handler for processing WiFi credentials.
 *
 * @param[in] req HTTP request object.
 * @return ESP_OK on success, or an error code on failure.
 */
static titan_err_t post_uri_wifi_credentials(httpd_req_t* req) {
    network_credentials credentials_proto;
    char ssid[32]     = {0};
    char password[64] = {0};
    titan_err_t result  = Error::UNKNOW_FAIL;

    HTTPServerProcess* http_server_manager =
        reinterpret_cast<HTTPServerProcess*>(req->user_ctx);

    uint8_t ssid_size =
        httpd_req_get_hdr_value_len(req, "my-connected-ssid") + 1;
    uint8_t pwd_size = httpd_req_get_hdr_value_len(req, "my-connected-pwd") + 1;

    do {
        if (ssid_size <= 0) {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                "SSID size equals to 0!");
            return Error::UNKNOW_FAIL;
        }

        if (pwd_size <= 0) {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                "Password size equals to 0!");
            return Error::UNKNOW_FAIL;
        }

        if (httpd_req_get_hdr_value_str(req, "my-connected-ssid", ssid,
                                        ssid_size) != ESP_OK) {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                "Error reading SSID!");
            return Error::UNKNOW_FAIL;
        }

        if (httpd_req_get_hdr_value_str(req, "my-connected-pwd", password,
                                        pwd_size) != ESP_OK) {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                                "Error reading Password!");
            return Error::UNKNOW_FAIL;
        }
        strcpy(credentials_proto.ssid, ssid);
        strcpy(credentials_proto.password, password);

        http_server_manager->memory_manager()->Write(MEMORY_AREAS_NETWORK_CREDENTIALS, credentials_proto, network_credentials_t_msg);
        result = ESP_OK;

    } while (0);

    return result;
}

/**
 * @brief Main execution loop for HTTPServerProcess.
 *
 * This function runs an infinite loop with a delay of 1000 milliseconds.
 */
void HTTPServerProcess::Execute(void) {
    if (this->Initialize() != Error::NO_ERROR) {
        vTaskDelete(this->_process_handler);
    }

    while (1) {
        do {
            this->_shared_memory_manager->Read(MEMORY_AREAS_NETWORK_INFORMATION,
                                               this->_connection_status,
                                               network_information_t_msg);
            auto ap_changed =
                this->_last_connection_status.ap_connected !=
                this->_connection_status.ap_connected;
            auto sta_changed =
                this->_last_connection_status.sta_connected !=
                this->_connection_status.sta_connected;

            if (!ap_changed && !sta_changed) {
                break;
            }

            auto ap_status  = this->_connection_status.ap_connected;
            auto sta_status = this->_connection_status.sta_connected;

            if ((ap_status == NETWORK_STATUS_CONNECTED) || (sta_status == NETWORK_STATUS_CONNECTED)) {
                if (this->_server_status != NETWORK_STATUS_CONNECTED) {
                    this->StartHTTPServer();
                    this->_server_status = NETWORK_STATUS_CONNECTED;
                    ESP_LOGI(TAG, "HTTP SERVER STARTED");
                }
            } else if ((ap_status == NETWORK_STATUS_DISCONNECTED) && (sta_status == NETWORK_STATUS_DISCONNECTED)) {
                if (this->_server_status != NETWORK_STATUS_CONNECTED) {
                    this->StopHTTPServer();
                    this->_server_status = NETWORK_STATUS_DISCONNECTED;
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
titan_err_t HTTPServerProcess::Initialize(void) {
    auto result = Error::UNKNOW_FAIL;

    this->_config                   = HTTPD_DEFAULT_CONFIG();
    this->_config.send_wait_timeout = 10;
    this->_config.recv_wait_timeout = 10;
    this->_config.max_uri_handlers  = 20;
    this->_shared_memory_manager    = SharedMemoryManager::GetInstance();

    result = this->InitializeRequestList();

    return result;
}

/**
 * @brief Starts the HTTP server and registers URI handlers.
 *
 * @return ESP_OK on success, or an error code on failure.
 */
titan_err_t HTTPServerProcess::StartHTTPServer(void) {
    return httpd_start(&this->_server, &this->_config);
}

/**
 * @brief Stops the HTTP server.
 *
 * @return ESP_OK on success, or an error code on failure.
 */
titan_err_t HTTPServerProcess::StopHTTPServer(void) {
    return httpd_stop(&this->_server);
}

/**
 * @brief Initializes the list of HTTP request URIs and their corresponding handlers.
 */
titan_err_t HTTPServerProcess::InitializeRequestList(void) {
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

    static const httpd_uri_t uri_get_proto = {
        .uri      = "/titanium.proto",
        .method   = HTTP_GET,
        .handler  = get_uri_proto_file,
        .user_ctx = this,
    };

    auto result = Error::NO_ERROR;
    result += httpd_register_uri_handler(this->_server, &uri_index_html);
    result += httpd_register_uri_handler(this->_server, &uri_get_styles_css);
    result += httpd_register_uri_handler(this->_server, &uri_get_app_js);
    result += httpd_register_uri_handler(this->_server, &uri_get_jquery_js);
    result += httpd_register_uri_handler(this->_server, &uri_get_favicon_ico);
    result += httpd_register_uri_handler(this->_server, &uri_post_credentials);
    result += httpd_register_uri_handler(this->_server, &uri_get_proto);

    return result;
}
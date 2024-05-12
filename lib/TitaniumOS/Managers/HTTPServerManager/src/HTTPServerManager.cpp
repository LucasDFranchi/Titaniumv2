// #include <HTTPServerManager.h>
// #include <stdlib.h>

// #include "./RequestHandler/RequestHandler.hpp"
// #include "MemoryUtils.h"
// #include "esp_log.h"

// static const char* TAG = "HTTPServerManager";

// namespace BinaryData {
// extern const uint8_t index_html_start[] asm("_binary_index_html_start");
// extern const uint8_t index_html_end[] asm("_binary_index_html_end");
// extern const uint8_t styles_css_start[] asm("_binary_styles_css_start");
// extern const uint8_t styles_css_end[] asm("_binary_styles_css_end");
// extern const uint8_t app_js_start[] asm("_binary_app_js_start");
// extern const uint8_t app_js_end[] asm("_binary_app_js_end");
// extern const uint8_t jquery3_js_start[] asm(
//     "_binary_jquery_3_3_1_min_js_start");
// extern const uint8_t jquery3_js_end[] asm("_binary_jquery_3_3_1_min_js_end");
// extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start");
// extern const uint8_t favicon_ico_end[] asm("_binary_favicon_ico_end");
// }  // namespace BinaryData

// static esp_err_t get_uri_index_html(httpd_req_t* req) {
//     httpd_resp_set_type(req, "text/html");
//     httpd_resp_send(req,
//                     reinterpret_cast<const char*>(BinaryData::index_html_start),
//                     BinaryData::index_html_end - BinaryData::index_html_start);

//     return ESP_OK;
// }

// static esp_err_t get_uri_get_app_css(httpd_req_t* req) {
//     httpd_resp_set_type(req, "text/css");
//     httpd_resp_send(req,
//                     reinterpret_cast<const char*>(BinaryData::styles_css_start),
//                     BinaryData::styles_css_end - BinaryData::styles_css_start);

//     return ESP_OK;
// }

// static esp_err_t get_uri_get_app_js(httpd_req_t* req) {
//     httpd_resp_set_type(req, "text/javascript");
//     httpd_resp_send(req,
//                     reinterpret_cast<const char*>(BinaryData::app_js_start),
//                     BinaryData::app_js_end - BinaryData::app_js_start - 1);

//     return ESP_OK;
// }

// static esp_err_t get_uri_get_jquery_js(httpd_req_t* req) {
//     httpd_resp_set_type(req, "text/javascript");
//     httpd_resp_send(
//         req, reinterpret_cast<const char*>(BinaryData::jquery3_js_start),
//         BinaryData::jquery3_js_end - BinaryData::jquery3_js_start - 1);

//     return ESP_OK;
// }

// static esp_err_t get_uri_favicon_icon(httpd_req_t* req) {
//     httpd_resp_set_type(req, "image/x-icon");
//     httpd_resp_send(
//         req, reinterpret_cast<const char*>(BinaryData::favicon_ico_start),
//         BinaryData::favicon_ico_end - BinaryData::favicon_ico_start);

//     return ESP_OK;
// }

// static esp_err_t post_uri_wifi_credentials(httpd_req_t* req) {
//     credentials_st credentials;
//     char ssid[32] = {0};
//     char password[64] = {0};
//     esp_err_t result = ESP_FAIL;

//     HTTPServerManager* http_server_manager =
//         reinterpret_cast<HTTPServerManager*>(req->user_ctx);

//     uint8_t ssid_size =
//         httpd_req_get_hdr_value_len(req, "my-connected-ssid") + 1;
//     uint8_t pwd_size = httpd_req_get_hdr_value_len(req, "my-connected-pwd") + 1;

//     do {
//         if (ssid_size <= 0) {
//             httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
//                                 "SSID size equals to 0!");
//             return ESP_FAIL;
//         }

//         if (pwd_size <= 0) {
//             httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
//                                 "Password size equals to 0!");
//             return ESP_FAIL;
//         }

//         if (httpd_req_get_hdr_value_str(req, "my-connected-ssid", ssid,
//                                         ssid_size) != ESP_OK) {
//             httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
//                                 "Error reading SSID!");
//             return ESP_FAIL;
//         }

//         if (httpd_req_get_hdr_value_str(req, "my-connected-pwd", password,
//                                         pwd_size) != ESP_OK) {
//             httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
//                                 "Error reading Password!");
//             return ESP_FAIL;
//         }

//         memcpy_s(credentials.sta_ssid, ssid, sizeof(credentials.sta_ssid));
//         memcpy_s(credentials.sta_password, password,
//                  sizeof(credentials.sta_password));

//         http_server_manager->memory_manager()->Write(
//             CREDENTIALS_AREA, sizeof(credentials), &credentials);
//         result = ESP_OK;

//     } while (0);

//     return result;
// }

// static esp_err_t get_area_handler(httpd_req_t* req) {
//     uint32_t response_size = 0;
//     uint16_t area_index = 0;

//     if (GetRequestKey(req, "/get_area?id=", &area_index)) {
//         httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Index out of range!");
//         return ESP_FAIL;
//     }

//     HTTPServerManager* http_server_manager =
//         reinterpret_cast<HTTPServerManager*>(req->user_ctx);

//     if (req->method == HTTP_GET) {
//         switch (area_index) {
//             case DISPLAY_AREA: {
//                 httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
//                                     "Private memory area!");
//             } break;
//             case CONNECTION_AREA: {
//                 response_size =
//                     GetConnectionArea(&http_server_manager->response_buffer[0],
//                                       http_server_manager->memory_manager());
//             } break;
//             case LORA_READ_AREA: {
//                 response_size =
//                     GetLoraReadArea(&http_server_manager->response_buffer[0],
//                                     http_server_manager->memory_manager());
//             } break;
//             case LORA_WRITE_AREA: {
//                 response_size =
//                     GetLoraWriteArea(&http_server_manager->response_buffer[0],
//                                      http_server_manager->memory_manager());
//             } break;
//             case CREDENTIALS_AREA: {
//                 response_size =
//                     GetCredentialsArea(&http_server_manager->response_buffer[0],
//                                        http_server_manager->memory_manager());
//             } break;
//             default: {
//                 httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
//                                     "Invalid Memory Area!");
//                 return ESP_FAIL;
//             }
//         }
//         if (response_size > 0) {
//             httpd_resp_send(req, http_server_manager->response_buffer,
//                             response_size);
//         }
//     } else if ((req->method == HTTP_POST)) {
//         esp_err_t result = ESP_FAIL;

//         if (GetRequestData(req, http_server_manager->read_buffer,
//                            sizeof(http_server_manager->read_buffer)) !=
//             ESP_OK) {
//             return ESP_FAIL;
//         }

//         switch (area_index) {
//             case DISPLAY_AREA: {
//                 result =
//                     http_server_manager->memory_manager()->Write<display_st>(
//                         DISPLAY_AREA, req->content_len,
//                         reinterpret_cast<display_st*>(
//                             http_server_manager->read_buffer));
//             } break;
//             case CONNECTION_AREA: {
//                 result =
//                     http_server_manager->memory_manager()->Write<connection_st>(
//                         CONNECTION_AREA, req->content_len,
//                         reinterpret_cast<connection_st*>(
//                             http_server_manager->read_buffer));
//             } break;
//             case LORA_READ_AREA: {
//                 result =
//                     http_server_manager->memory_manager()->Write<lora_read_st>(
//                         LORA_READ_AREA, req->content_len,
//                         reinterpret_cast<lora_read_st*>(
//                             http_server_manager->read_buffer));
//             } break;
//             case LORA_WRITE_AREA: {
//                 result =
//                     http_server_manager->memory_manager()->Write<lora_write_st>(
//                         LORA_WRITE_AREA, req->content_len,
//                         reinterpret_cast<lora_write_st*>(
//                             http_server_manager->read_buffer));
//             } break;
//             case CREDENTIALS_AREA: {
//                 result = http_server_manager->memory_manager()
//                              ->Write<credentials_st>(
//                                  CREDENTIALS_AREA, req->content_len,
//                                  reinterpret_cast<credentials_st*>(
//                                      http_server_manager->read_buffer));
//             } break;
//             default: {
//                 httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
//                                     "Invalid Memory Area!");
//                 return ESP_FAIL;
//             }
//         }

//         if (result == ESP_OK) {
//             httpd_resp_sendstr_chunk(req, "OK");
//         } else {
//             httpd_resp_sendstr_chunk(req, "NOK");
//         }

//         httpd_resp_sendstr_chunk(req, NULL);
//     }

//     return ESP_OK;
// }

// /**
//  * The function 'Execute' runs an infinite loop with a delay of 1000
//  * milliseconds.
//  */
// void HTTPServerManager::Execute(void) {
//     this->Initialize();

//     while (1) {
//         if (this->_memory_manager->IsAreaDataNew(CONNECTION_AREA)) {
//             this->_memory_manager->Read(CONNECTION_AREA,
//                                         &this->_connection_status);
//         }

//         do {
//             auto ap_changed =
//                 this->_last_connection_status.connection_ap_status !=
//                 this->_connection_status.connection_ap_status;
//             auto sta_changed =
//                 this->_last_connection_status.connection_sta_status !=
//                 this->_connection_status.connection_sta_status;

//             if (!ap_changed && !sta_changed) {
//                 break;
//             }

//             auto ap_status = this->_connection_status.connection_ap_status;
//             auto sta_status = this->_connection_status.connection_sta_status;

//             if ((ap_status == NetworkStatus::CONNECTED) || (sta_status == NetworkStatus::CONNECTED)) {
//                 if (this->_server_status != NetworkStatus::CONNECTED) {
//                     this->StartHTTPServer();
//                     this->_server_status = NetworkStatus::CONNECTED;
//                     ESP_LOGI(TAG, "HTTP SERVER STARTED");
//                 }
//             }
//             else if ((ap_status == NetworkStatus::NOT_CONNECTED) && (sta_status == NetworkStatus::NOT_CONNECTED)) {
//                 if (this->_server_status != NetworkStatus::CONNECTED) {
//                     this->StopHTTPServer();
//                     this->_server_status = NetworkStatus::NOT_CONNECTED;
//                     ESP_LOGI(TAG, "HTTP SERVER STOPED");
//                 }
//             }
//         } while (0);

//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }

// /**
//  * The function initializes the HTTPManager by reading files from SPIFFS and
//  * initializing the request list.
//  *
//  * @return an 'esp_err_t' value.
//  */
// esp_err_t HTTPServerManager::Initialize(void) {
//     auto result = ESP_OK;

//     this->_config = HTTPD_DEFAULT_CONFIG();
//     this->_config.send_wait_timeout = 10;
//     this->_config.recv_wait_timeout = 10;
//     this->_config.max_uri_handlers = 20;
//     this->_memory_manager = MemoryManager::GetInstance();

//     this->InitializeRequestList();

//     return result;
// }

// /**
//  * The function starts an HTTP server and registers handlers.
//  *
//  * @return an `esp_err_t` value.
//  */
// esp_err_t HTTPServerManager::StartHTTPServer(void) {
//     auto result = ESP_OK;

//     if (httpd_start(&this->_server, &this->_config) == ESP_OK) {
//         result += RegisterHandlers();
//     }

//     return result;
// }

// /**
//  * The function `StopHTTPServer` stops the HTTP server.
//  *
//  * @return an esp_err_t, which is a type defined in the ESP-IDF framework for
//  * error handling.
//  */
// esp_err_t HTTPServerManager::StopHTTPServer(void) {
//     return httpd_stop(&this->_server);
// }

// /**
//  * The function InitializeRequestList_ initializes a list of HTTP request URIs
//  * and their corresponding handlers.
//  */
// void HTTPServerManager::InitializeRequestList(void) {
//     static const httpd_uri_t uri_get_area_get = {
//         .uri = "/get_area",
//         .method = HTTP_GET,
//         .handler = get_area_handler,
//         .user_ctx = this,
//     };

//     static const httpd_uri_t uri_get_area_post = {
//         .uri = "/get_area",
//         .method = HTTP_POST,
//         .handler = get_area_handler,
//         .user_ctx = this,
//     };

//     static const httpd_uri_t uri_index_html = {
//         .uri = "/",
//         .method = HTTP_GET,
//         .handler = get_uri_index_html,
//         .user_ctx = this,
//     };

//     static const httpd_uri_t uri_get_styles_css = {
//         .uri = "/styles.css",
//         .method = HTTP_GET,
//         .handler = get_uri_get_app_css,
//         .user_ctx = this,
//     };

//     static const httpd_uri_t uri_get_app_js = {
//         .uri = "/app.js",
//         .method = HTTP_GET,
//         .handler = get_uri_get_app_js,
//         .user_ctx = this,
//     };

//     static const httpd_uri_t uri_get_jquery_js = {
//         .uri = "/jquery-3.3.1.min.js",
//         .method = HTTP_GET,
//         .handler = get_uri_get_jquery_js,
//         .user_ctx = this,
//     };

//     static const httpd_uri_t uri_get_favicon_ico = {
//         .uri = "/favicon.ico",
//         .method = HTTP_GET,
//         .handler = get_uri_favicon_icon,
//         .user_ctx = this,
//     };

//     static const httpd_uri_t uri_post_credentials = {
//         .uri = "/wifiCredentials.json",
//         .method = HTTP_POST,
//         .handler = post_uri_wifi_credentials,
//         .user_ctx = this,
//     };

//     // This list is a pain in the ass to maintain
//     this->_http_requests_list[0] = uri_get_area_get;
//     this->_http_requests_list[1] = uri_get_area_post;
//     this->_http_requests_list[2] = uri_index_html;
//     this->_http_requests_list[3] = uri_get_styles_css;
//     this->_http_requests_list[4] = uri_get_app_js;
//     this->_http_requests_list[5] = uri_get_jquery_js;
//     this->_http_requests_list[6] = uri_get_favicon_ico;
//     this->_http_requests_list[7] = uri_post_credentials;
// }

// /**
//  * The function iterates through a list of HTTP request handlers and registers
//  * them with an HTTP server.
//  *
//  * @return an 'esp_err_t' value.
//  */
// esp_err_t HTTPServerManager::RegisterHandlers(void) {
//     auto result = ESP_OK;

//     for (int i = 0; i < this->maximum_requests_list_size; i++) {
//         result += httpd_register_uri_handler(this->_server,
//                                              &this->_http_requests_list[i]);
//     }

//     return result;
// }
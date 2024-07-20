#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include "Kernel/error/error_enum.h"
#include "esp_err.h"
#include "esp_http_server.h"

namespace Request {
    /**
     * @brief Maximum size of the request reply buffer.
     */
    constexpr uint16_t MAXIMUM_REQUEST_REPLY = 1024;
}  // namespace Request

titan_err_t GetRequestKey(httpd_req_t* req, const char* buffer, uint16_t* key);
titan_err_t GetRequestData(httpd_req_t* req, char* buffer, uint16_t buffer_size);

#endif /* REQUEST_HANDLER_H */
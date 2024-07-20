#ifndef ERROR_H
#define ERROR_H

#include "esp_err.h"

typedef int titan_err_t;

namespace Error {
    constexpr titan_err_t SUCCESS                  = 0;
    constexpr titan_err_t INVALID_START_BYTE       = -1; /**< Error code indicating invalid Start Byte. */
    constexpr titan_err_t INVALID_PAYLOAD_SIZE     = -2; /**< Error code indicating invalid payload size. */
    constexpr titan_err_t INVALID_COMMAND          = -3; /**< Error code indicating invalid command. */
    constexpr titan_err_t INVALID_MEMORY_AREA      = -4; /**< Error code indicating invalid memory area. */
    constexpr titan_err_t INVALID_ADDRESS          = -5; /**< Error code indicating invalid address. */
    constexpr titan_err_t INVALID_PAYLOAD_POINTER  = -6; /**< Error code indicating invalid payload pointer. */
    constexpr titan_err_t INVALID_CRC              = -7; /**< Error code indicating invalid CRC. */
    constexpr titan_err_t INVALID_END_BYTE         = -8; /**< Error code indicating invalid End Byte. */
    constexpr titan_err_t INVALID_UUID             = -9; /**< Error code indicating invalid UUID. */
    constexpr titan_err_t UNKNOW_FAIL              = -10;
    constexpr titan_err_t NULL_PTR                 = -11;
    constexpr titan_err_t BUFFER_OUT_OF_SPACE      = -12;
    constexpr titan_err_t PACKAGE_GENERATION_ERROR = -13;
    constexpr titan_err_t PACKAGE_DECODE_ERROR     = -14;
    constexpr titan_err_t PACKAGE_ENCODE_ERROR     = -15;
    constexpr titan_err_t READ_FAIL                = -16;
    constexpr titan_err_t SERIALIZE_JSON_ERROR     = -17;
    constexpr titan_err_t CONSUME_ERROR            = -18;
    constexpr titan_err_t SERIALIZE_ERROR          = -19;
    constexpr titan_err_t DESERIALIZE_ERROR        = -20;
}  // namespace Error

#endif /* ERROR_H */
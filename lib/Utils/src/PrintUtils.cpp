#include "PrintUtils.h"

#include <stdarg.h>
#include <stdio.h>

uint32_t snprintf_s(char* buffer, size_t size, const char* format, ...) {
    va_list args;
    uint32_t written_chars = 0;

    if (buffer == nullptr) return 0;

    va_start(args, format);
    written_chars = vsnprintf(buffer, size, format, args);
    va_end(args);

    if (size > 0) {
        buffer[size - 1] = '\0';
    }

    return written_chars;
}

uint32_t snprintf_array(char* buffer, uint8_t* arr, uint32_t arr_size,
                        uint32_t max_size) {
    uint32_t offset = 0;

    if (buffer == nullptr) return 0;

    if (arr == nullptr) return 0;

    offset += snprintf_s(buffer, max_size, "[");

    for (uint32_t i = 0; i < arr_size; i++) {
        offset += snprintf_s(&buffer[offset], max_size - offset, "%d", arr[i]);

        if (i < arr_size - 1) {
            offset += snprintf_s(&buffer[offset], max_size - offset, ",");
        }
    }

    offset += snprintf_s(&buffer[offset], max_size - offset, "]");

    return offset;
}
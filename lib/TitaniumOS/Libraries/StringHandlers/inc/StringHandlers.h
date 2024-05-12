#ifndef STRING_HANDLERS
#define STRING_HANDLERS

#include <cstdarg>
#include <cstdint>
#include <cstdio>

/**
 * @brief Safely formats a string using vsnprintf.
 * 
 * Ensures null-termination and returns the number of characters
 * written (excluding the null terminator).
 * 
 * @param[out] buffer Pointer to the destination buffer.
 * @param[in] size Maximum number of characters (including the null terminator) that can be written to buffer.
 * @param[in] format Format string (same as snprintf).
 * @param ... Optional arguments to format (same as snprintf).
 * 
 * @return Number of characters written (excluding the null terminator) if successful, or 0 on failure.
 */
uint32_t snprintf_s(char* buffer, size_t size, const char* format, ...) {
    va_list args;
    uint32_t written_chars = 0;

    if (buffer == nullptr || size == 0) {
        return 0; // Invalid arguments
    }

    va_start(args, format);
    written_chars = vsnprintf(buffer, size, format, args);
    va_end(args);

    // Ensure null termination
    buffer[size - 1] = '\0';

    if (written_chars >= size) {
        return 0; // Error: snprintf truncation
    }

    return written_chars;
}

/**
 * @brief Formats an array into a string representation.
 * 
 * Formats the array into a comma-separated list enclosed in square brackets.
 * 
 * @param[out] buffer Pointer to the destination buffer.
 * @param[in] arr Pointer to the array of integers.
 * @param[in] arr_size Number of elements in the array.
 * @param[in] max_size Maximum size of the destination buffer.
 * 
 * @return Number of characters written to buffer if successful, or 0 on failure.
 */
uint32_t snprintf_array(char* buffer, const uint8_t* arr, uint32_t arr_size, uint32_t max_size) {
    uint32_t offset = 0;

    if (buffer == nullptr || arr == nullptr || max_size == 0) {
        return 0; // Invalid arguments
    }

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


#endif /* STRING_HANDLERS */
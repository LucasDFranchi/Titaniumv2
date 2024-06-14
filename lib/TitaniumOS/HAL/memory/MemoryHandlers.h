#ifndef __MEMORY_HANDLER_GUARD__
#define __MEMORY_HANDLER_GUARD__

#include "esp_err.h"

/**
 * The function 'memcpy_s' copies a specified number of bytes from one memory
 * location to another, while performing checks for invalid sizes and memory
 * availability.
 *
 * @param pOut[out] A pointer to the destination buffer where the copied data
 * will be stored.
 * @param pIn[in] A pointer to the source memory location from where the data
 * will be copied.
 * @param size[in] The size parameter represents the number of bytes to be
 * copied from the source (pIn) to the destination (pOut).
 *
 * @return an 'esp_err_t' value, which is a type defined in the ESP-IDF
 * framework. The possible return values are 'ESP_OK', 'ESP_ERR_INVALID_SIZE',
 * and 'ESP_ERR_NO_MEM'.
 */
template <typename T>
esp_err_t memcpy_s(T* pOut, T* pIn, uint32_t size_in_bytes) {
    esp_err_t result = ESP_OK;

    do {
        if (size_in_bytes == 0) {
            result = ESP_ERR_INVALID_SIZE;
            break;
        }
        if (pOut == nullptr) {
            result = ESP_ERR_NO_MEM;
            break;
        }
        if (pIn == nullptr) {
            result = ESP_ERR_NO_MEM;
            break;
        }

        uint8_t* internal_input_pointer = reinterpret_cast<uint8_t*>(pIn);
        uint8_t* internal_output_pointer = reinterpret_cast<uint8_t*>(pOut);

        for (int i = 0; i < size_in_bytes; i++) {
            internal_output_pointer[i] = internal_input_pointer[i];
        }
    } while (0);

    return result;
}
/**
 * @brief Compares two memory buffers for equality.
 *
 * This function compares two memory buffers, pOut and pIn, of size
 * size_in_bytes for equality.
 *
 * @param[out] pOut Pointer to the destination memory buffer.
 * @param[in] pIn Pointer to the source memory buffer.
 * @param[in] size_in_bytes Size of the memory buffers in bytes.
 * @return esp_err_t Error code indicating the result of the comparison.
 *         - ESP_OK if the memory buffers are equal.
 *         - ESP_ERR_INVALID_SIZE if size_in_bytes is 0.
 *         - ESP_ERR_NO_MEM if either pOut or pIn is nullptr.
 *         - ESP_FAIL if the memory buffers are not equal.
 */
template <typename T>
esp_err_t memcmp_s(T* pOut, T* pIn, uint32_t size_in_bytes) {
    esp_err_t result = ESP_OK;

    do {
        if (size_in_bytes == 0) {
            result = ESP_ERR_INVALID_SIZE;
            break;
        }
        if (pOut == nullptr) {
            result = ESP_ERR_NO_MEM;
            break;
        }
        if (pIn == nullptr) {
            result = ESP_ERR_NO_MEM;
            break;
        }

        uint8_t* internal_input_pointer = reinterpret_cast<uint8_t*>(pIn);
        uint8_t* internal_output_pointer = reinterpret_cast<uint8_t*>(pOut);

        for (int i = 0; i < size_in_bytes; i++) {
            if (internal_output_pointer[i] != internal_input_pointer[i]) {
                result = ESP_FAIL;
                break;
            }
        }
    } while (0);

    return result;
}

/**
 * The function 'memset_s' sets a specified value to a block of memory.
 *
 * @param pOut[out] A pointer to the memory location where the memset operation
 * will be performed.
 * @param value[in] The value to be set in each byte of the memory block.
 * @param size_in_bytes[in] The size parameter represents the number of bytes to
 * be set in the memory block pointed to by pOut.
 *
 * @return an 'esp_err_t' value.
 */
template <typename T>
esp_err_t memset_s(T* pOut, uint8_t value, uint32_t size_in_bytes) {
    esp_err_t result = ESP_OK;

    do {
        if (size_in_bytes == 0) {
            result = ESP_ERR_INVALID_SIZE;
            break;
        }
        if (pOut == nullptr) {
            result = ESP_ERR_NO_MEM;
            break;
        }

        uint8_t* internal_output_pointer = reinterpret_cast<uint8_t*>(pOut);

        for (int i = 0; i < size_in_bytes; i++) {
            internal_output_pointer[i] = value;
        }

    } while (0);

    return result;
}

#endif /* __MEMORY_HANDLER_GUARD__ */
#ifndef MEMORY_HANDLER_H
#define MEMORY_HANDLER_H

#include "esp_err.h"

/**
 * @brief Copies a specified number of bytes from one memory location to another.
 *
 * This function copies size_in_bytes bytes of data from the source memory
 * location pIn to the destination memory location pOut, while performing checks
 * for invalid sizes and memory availability.
 *
 * @tparam T Type of the elements in the memory buffer.
 * @param[out] pOut Pointer to the destination memory buffer.
 * @param[in] pIn Pointer to the source memory buffer.
 * @param[in] size_in_bytes Size of the memory buffers in bytes.
 * @return esp_err_t Error code indicating the result of the operation.
 *         - ESP_OK if the memory copy operation is successful.
 *         - ESP_ERR_INVALID_SIZE if size_in_bytes is 0.
 *         - ESP_ERR_NO_MEM if either pOut or pIn is nullptr.
 */
template <typename T>
esp_err_t memcpy_s(T* pOut, T* pIn, uint32_t size_in_bytes) {
    esp_err_t result = ESP_OK;

    do {
        if (size_in_bytes == 0) {
            result = ESP_ERR_INVALID_SIZE;
            break;
        }
        if (pOut == nullptr || pIn == nullptr) {
            result = ESP_ERR_NO_MEM;
            break;
        }

        uint8_t* internal_input_pointer  = reinterpret_cast<uint8_t*>(pIn);
        uint8_t* internal_output_pointer = reinterpret_cast<uint8_t*>(pOut);

        for (uint32_t i = 0; i < size_in_bytes; i++) {
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
 * @tparam T Type of the elements in the memory buffer.
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
        if (pOut == nullptr || pIn == nullptr) {
            result = ESP_ERR_NO_MEM;
            break;
        }

        uint8_t* internal_input_pointer  = reinterpret_cast<uint8_t*>(pIn);
        uint8_t* internal_output_pointer = reinterpret_cast<uint8_t*>(pOut);

        for (uint32_t i = 0; i < size_in_bytes; i++) {
            if (internal_output_pointer[i] != internal_input_pointer[i]) {
                result = ESP_FAIL;
                break;
            }
        }
    } while (0);

    return result;
}

/**
 * @brief Sets a specified value to a block of memory.
 *
 * This function sets size_in_bytes bytes of the memory block pointed to by pOut
 * to the specified value, while performing checks for invalid sizes and memory
 * availability.
 *
 * @tparam T Type of the elements in the memory buffer.
 * @param[out] pOut Pointer to the memory location where the memset operation
 * will be performed.
 * @param[in] value The value to be set in each byte of the memory block.
 * @param[in] size_in_bytes Size of the memory block in bytes.
 * @return esp_err_t Error code indicating the result of the operation.
 *         - ESP_OK if the memset operation is successful.
 *         - ESP_ERR_INVALID_SIZE if size_in_bytes is 0.
 *         - ESP_ERR_NO_MEM if pOut is nullptr.
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

        for (uint32_t i = 0; i < size_in_bytes; i++) {
            internal_output_pointer[i] = value;
        }

    } while (0);

    return result;
}

#endif /* MEMORY_HANDLER_H */
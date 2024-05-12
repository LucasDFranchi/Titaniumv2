// #include "MemoryUtils.h"

// /**
//  * The function `memcpy_s` copies a specified number of bytes from one memory
//  * location to another, while performing checks for invalid sizes and memory
//  * availability.
//  *
//  * @param pOut A pointer to the destination buffer where the copied data will be
//  * stored.
//  * @param pIn A pointer to the source memory location from where the data will
//  * be copied.
//  * @param size The size parameter represents the number of bytes to be copied
//  * from the source (pIn) to the destination (pOut).
//  *
//  * @return an `esp_err_t` value, which is a type defined in the ESP-IDF
//  * framework. The possible return values are `ESP_OK`, `ESP_ERR_INVALID_SIZE`,
//  * and `ESP_ERR_NO_MEM`.
//  */
// esp_err_t memcpy_s(void* pOut, void* pIn, uint32_t size) {
//     esp_err_t result = ESP_OK;
//     uint8_t* internal_input_pointer = reinterpret_cast<uint8_t*>(pIn);
//     uint8_t* internal_output_pointer = reinterpret_cast<uint8_t*>(pOut);

//     if (size == 0) return ESP_ERR_INVALID_SIZE;
//     if (pOut == nullptr) return ESP_ERR_NO_MEM;
//     if (pIn == nullptr) return ESP_ERR_NO_MEM;

//     for (int i = 0; i < size; i++) {
//         internal_output_pointer[i] = internal_input_pointer[i];
//     }

//     return result;
// }
// /**
//  * The function `memset_s` sets a specified value to a block of memory.
//  *
//  * @param pOut A pointer to the memory location where the memset operation will
//  * be performed.
//  * @param value The value to be set in each byte of the memory block.
//  * @param size The size parameter represents the number of bytes to be set in
//  * the memory block pointed to by pOut.
//  *
//  * @return an `esp_err_t` value.
//  */

// esp_err_t memset_s(void* pOut, uint8_t value, uint32_t size) {
//     esp_err_t result = ESP_OK;
//     uint8_t* internal_output_pointer = reinterpret_cast<uint8_t*>(pOut);

//     if (size == 0) return ESP_ERR_INVALID_SIZE;
//     if (pOut == nullptr) return ESP_ERR_NO_MEM;

//     for (int i = 0; i < size; i++) {
//         internal_output_pointer[i] = value;
//     }

//     return result;
// }
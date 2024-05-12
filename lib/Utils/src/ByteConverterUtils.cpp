// #include "ByteConverterUtils.h"

// /**
//  * @brief Converts an array of bytes to a 16-bit unsigned integer.
//  *
//  * This function takes an array of bytes and interprets them as a little-endian
//  * 16-bit unsigned integer, converting it to the corresponding value.
//  *
//  * @param initial_byte_address A pointer to the initial byte of the array.
//  * @return The 16-bit unsigned integer value.
//  * @note If `initial_byte_address` is nullptr, the function returns 0.
//  */
// uint16_t ConvertBytesToUInt16(uint8_t *initial_byte_address) {
//     uint16_t result = 0;
//     for (uint8_t i = 0; i < sizeof(uint16_t); i++) {
//         auto address = &initial_byte_address[i];
//         if (address == nullptr) {
//             return -1;
//         }
//         result |= static_cast<uint16_t>(initial_byte_address[i])
//                   << (8 * (sizeof(uint16_t) - 1 - i));
//     }

//     return result;
// }

// uint32_t ConvertBytesToUInt32(uint8_t *initial_byte_address) {
//     uint32_t result = 0;
//     for (uint8_t i = 0; i < sizeof(uint32_t); i++) {
//         auto address = &initial_byte_address[i];
//         if (address == nullptr) {
//             return -1;
//         }
//         result |= static_cast<uint32_t>(initial_byte_address[i])
//                   << (8 * (sizeof(uint32_t) - 1 - i));
//     }

//     return result;
// }
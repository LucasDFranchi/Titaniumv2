#include "ByteConverterUtils.h"

uint16_t ConvertBytesToUInt16(uint8_t *initial_byte_address){  
    uint16_t result = 0;
    for (uint8_t i = 0; i < sizeof(uint16_t); i++) {
        if (&initial_byte_address[i] == nullptr) {
            return -1;
        }
        result |= static_cast<uint16_t>(initial_byte_address[i]) << (8 * (sizeof(uint16_t) - 1 - i));
    }

    return result;
}

uint32_t ConvertBytesToUInt32(uint8_t *initial_byte_address) {
    uint32_t result = 0;
    for (uint8_t i = 0; i < sizeof(uint32_t); i++) {
        if (&initial_byte_address[i] == nullptr) {
            return -1;
        }
        result |= static_cast<uint32_t>(initial_byte_address[i]) << (8 * (sizeof(uint32_t) - 1 - i));
    }

    return result;
}
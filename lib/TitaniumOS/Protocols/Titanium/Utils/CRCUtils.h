#ifndef CRC_UTILS_H
#define CRC_UTILS_H

#include <stdint.h>

uint32_t CalculatedCRC32(uint8_t *initial_byte_address, uint32_t size);

#endif /* CRC_UTILS_H */
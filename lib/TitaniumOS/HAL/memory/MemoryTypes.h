#ifndef MEMORY_TYPES_H
#define MEMORY_TYPES_H

#include <stdint.h>

/**
 * @brief Enumeration of access types for a resource.
 */
enum AccessType : uint8_t {
    READ_ONLY = 0, /**< Read-only access type. */
    WRITE_ONLY,    /**< Write-only access type. */
    READ_WRITE,    /**< Read-write access type. */
};

#endif /* MEMORY_TYPES_H */

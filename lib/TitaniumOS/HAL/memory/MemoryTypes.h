#ifndef MEMORY_TYPES_GUARD
#define MEMORY_TYPES_GUARD

#include <stdint.h>

/**
 * @brief Enumeration of access types for a resource.
 */
enum AccessType : uint8_t {
    READ_ONLY = 0,
    WRITE_ONLY,
    READ_WRITE,
};

#endif /* MEMORY_TYPES_GUARD */
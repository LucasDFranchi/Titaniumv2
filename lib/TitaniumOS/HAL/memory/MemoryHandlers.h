#ifndef MEMORY_HANDLER_H
#define MEMORY_HANDLER_H

#include "Kernel/error/error_enum.h"

titan_err_t memcpy_s(void* pOut, void* pIn, uint32_t size_in_bytes);
titan_err_t memcmp_s(void* pOut, void* pIn, uint32_t size_in_bytes);
titan_err_t memset_s(void* pOut, uint8_t value, uint32_t size_in_bytes);

#endif /* MEMORY_HANDLER_H */
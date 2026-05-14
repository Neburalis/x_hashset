#ifndef HASHFUNCS_H
#define HASHFUNCS_H

#include "stringNthong.h"

namespace hash {

unsigned long why_not(const x_str::x_str_t *str);
unsigned long first_char(const x_str::x_str_t *str);
unsigned long len(const x_str::x_str_t *str);
unsigned long sum(const x_str::x_str_t *str);
unsigned long rol(const x_str::x_str_t *str);
unsigned long ror(const x_str::x_str_t *str);
unsigned long crc32(const x_str::x_str_t *str);
unsigned long crc32_hw(const x_str::x_str_t *str);         // hardware CRC32 (SSE4.2)
unsigned long crc32_hw_aligned(const x_str::x_str_t *str); // unrolled for 32-byte aligned slots

uint64_t crc32_sw_raw(const char *str, size_t len);         // raw SW CRC32, no x_str_t
uint64_t crc32_hw_raw(const char *str, size_t len);         // raw HW CRC32, no x_str_t
uint64_t crc32_hw_aligned32(const char *slot);              // unrolled HW CRC32, 32-byte aligned slot

}

#endif // HASHFUNCS_H
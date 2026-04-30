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
unsigned long crc32_hw(const x_str::x_str_t *str);  // hardware CRC32 (SSE4.2)

}

#endif // HASHFUNCS_H
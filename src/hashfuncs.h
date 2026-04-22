#ifndef HASHFUNCS_H
#define HASHFUNCS_H

namespace hash {

unsigned long why_not(const x_str_t *str);
unsigned long first_char(const x_str_t *str);
unsigned long len(const x_str_t *str);
unsigned long sum(const x_str_t *str);
unsigned long rol(const x_str_t *str);
unsigned long ror(const x_str_t *str);
unsigned long crc32(const x_str_t *str);
unsigned long gnu(const x_str_t *str);
unsigned long sha256(const x_str_t *str);

}

#endif // HASHFUNCS_H
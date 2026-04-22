#include <stdint.h>

#include "hashfuncs.h"
#include "stringNthong.h"

using namespace x_str;

namespace hash {

unsigned long why_not(const x_str_t *str) {
    return 1;
}

unsigned long first_char(const x_str_t *str) {
    return str->str[0];
}

unsigned long len(const x_str_t *str) {
    return str->len;
}

unsigned long sum(const x_str_t *str) {
    unsigned long hash = 0;
    for (size_t i = 0; i < str->len; ++i) {
        hash += str->str[i];
    }
    return hash;
}

unsigned long rol(const x_str_t *str) {
    unsigned long hash = 0;
    for (size_t i = 0; i < str->len; ++i) {
        //TODO: rol hash
        hash ^= str->str[i];
    }
    return hash;
}

unsigned long ror(const x_str_t *str) {
    unsigned long hash = 0;
    for (size_t i = 0; i < str->len; ++i) {
        //TODO: ror hash
        hash ^= str->str[i];
    }
    return hash;
}

unsigned long crc32(const x_str_t *str);

unsigned long gnu(const x_str_t *str);

unsigned long sha256(const x_str_t *str);

unsigned long djb2(const x_str_t *str) {
    return djb2_impl(str && str->str ? str->str : nullptr);
}

unsigned long sdbm(const x_str_t *str) {
    return hash_sdbm_impl(str && str->str ? str->str : nullptr);
}

static unsigned long djb2_impl(const char *s) {
    unsigned long h = 5381;
    if (!s) return 0;
    for (unsigned char c; (c = (unsigned char)(*s++));) h = ((h << 5) + h) + c;
    return h ? h : 1;
}

static unsigned long hash_sdbm_impl(const char *s) {
    unsigned long h = 0;
    if (!s) return 1;
    for (unsigned char c; (c = (unsigned char)(*s++));) h = c + (h << 6) + (h << 16) - h;
    return h ? h : 1;
}

}
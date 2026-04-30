#include <stdint.h>
#include <string.h>
#include <nmmintrin.h>  // SSE4.2: _mm_crc32_u8, _mm_crc32_u32

#include "hashfuncs.h"
#include "stringNthong.h"

using namespace x_str;

namespace hash {

static inline unsigned long rol64(unsigned long x, unsigned int n) {
    return (x << n) | (x >> (64 - n));
}

static inline unsigned long ror64(unsigned long x, unsigned int n) {
    return (x >> n) | (x << (64 - n));
}

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
        hash = rol64(hash, 1);
        hash ^= str->str[i];
    }
    return hash;
}

unsigned long ror(const x_str_t *str) {
    unsigned long hash = 0;
    for (size_t i = 0; i < str->len; ++i) {
        hash = ror64(hash, 1);
        hash ^= str->str[i];
    }
    return hash;
}

unsigned long crc32(const x_str_t *s) {
    if (!s || !s->str) {
        return 0;
    }

    uint32_t crc = 0xFFFFFFFFu;

    for (uint64_t i = 0; i < s->len; ++i) {
        crc ^= (uint8_t)s->str[i];

        for (int bit = 0; bit < 8; ++bit) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320u;
            } else {
                crc >>= 1;
            }
        }
    }

    return (unsigned long)(crc ^ 0xFFFFFFFFu);
}

unsigned long crc32_hw(const x_str_t *s) {
    if (!s || !s->str) return 0;

    uint32_t crc = 0xFFFFFFFFu;
    const uint8_t *p = (const uint8_t *)s->str;
    uint64_t n = s->len;

    while (n >= 4) {
        uint32_t chunk;
        memcpy(&chunk, p, 4);
        crc = _mm_crc32_u32(crc, chunk);
        p += 4;
        n -= 4;
    }
    while (n--) {
        crc = _mm_crc32_u8(crc, *p++);
    }

    return (unsigned long)(crc ^ 0xFFFFFFFFu);
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

unsigned long djb2(const x_str_t *str) {
    return djb2_impl((str && str->str) ? str->str : nullptr);
}

unsigned long sdbm(const x_str_t *str) {
    return hash_sdbm_impl((str && str->str) ? str->str : nullptr);
}

}
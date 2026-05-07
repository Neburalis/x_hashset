#pragma once
#include <stddef.h>

#ifdef __AVX2__
#include <immintrin.h>
#else
#include <string.h>
#endif

static inline bool simd_memeq_slot32(const char *a, const char *b) {
#ifdef __AVX2__
    __m256i va = _mm256_load_si256((const __m256i *)a);
    __m256i vb = _mm256_load_si256((const __m256i *)b);
    return _mm256_movemask_epi8(_mm256_cmpeq_epi8(va, vb)) == (int)0xFFFFFFFF;
#else
    return memcmp(a, b, 32) == 0;
#endif
}

static inline size_t simd_strlen(const char *s) {
    const char *p = s;

#ifdef __AVX2__
    while ((uintptr_t)p & 31) {
        if (*p == '\0') return (size_t)(p - s);
        p++;
    }
    const __m256i zero = _mm256_setzero_si256();
    for (;;) {
        __m256i v    = _mm256_load_si256((const __m256i *)p);
        __m256i cmp  = _mm256_cmpeq_epi8(v, zero);
        int     mask = _mm256_movemask_epi8(cmp);
        if (mask)
            return (size_t)(p - s) + (size_t)__builtin_ctz(mask);
        p += 32;
    }
#endif
    while (*p) p++;
    return (size_t)(p - s);
}

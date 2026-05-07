#pragma once
#include <stddef.h>

#ifdef __SSE2__
#include <emmintrin.h>
#endif
#ifdef __AVX2__
#include <immintrin.h>
#endif

// Compares exactly `len` bytes of `a` and `b`.
// Returns true if all bytes are equal.
static inline bool simd_memeq(const char *a, const char *b, size_t len) {
    size_t i = 0;

#ifdef __AVX2__
    for (; i + 32 <= len; i += 32) {
        __m256i va   = _mm256_loadu_si256((const __m256i *)(a + i));
        __m256i vb   = _mm256_loadu_si256((const __m256i *)(b + i));
        __m256i eq   = _mm256_cmpeq_epi8(va, vb);
        int     mask = _mm256_movemask_epi8(eq);
        if (mask != (int)0xFFFFFFFF)
            return false;
    }
#endif

#ifdef __SSE2__
    for (; i + 16 <= len; i += 16) {
        __m128i va   = _mm_loadu_si128((const __m128i *)(a + i));
        __m128i vb   = _mm_loadu_si128((const __m128i *)(b + i));
        __m128i eq   = _mm_cmpeq_epi8(va, vb);
        int     mask = _mm_movemask_epi8(eq);
        if (mask != 0xFFFF)
            return false;
    }
#endif

    for (; i < len; i++)
        if (a[i] != b[i]) return false;
    return true;
}

// Returns the length of the null-terminated string `s`.
// Uses 16-byte aligned SSE2 loads; reading within an aligned 16-byte block
// never crosses a page boundary, so it is safe even near the end of the string.
static inline size_t simd_strlen(const char *s) {
    const char *p = s;

#ifdef __SSE2__
    while ((uintptr_t)p & 15) {
        if (*p == '\0') return (size_t)(p - s);
        p++;
    }
    const __m128i zero = _mm_setzero_si128();
    for (;;) {
        __m128i v    = _mm_load_si128((const __m128i *)p);
        __m128i cmp  = _mm_cmpeq_epi8(v, zero);
        int     mask = _mm_movemask_epi8(cmp);
        if (mask)
            return (size_t)(p - s) + (size_t)__builtin_ctz(mask);
        p += 16;
    }
#else
    while (*p) p++;
    return (size_t)(p - s);
#endif
}

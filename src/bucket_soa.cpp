#include "bucket_soa.h"
#include <stdlib.h>
#include <string.h>

#ifdef __AVX2__
#include <immintrin.h>
#endif

namespace bucket_soa {

bucket_t construct(size_t initial_capacity) {
    bucket_t b;
    b.hashes   = (uint64_t *)calloc(initial_capacity, sizeof(uint64_t));
    b.lens     = (size_t *)  calloc(initial_capacity, sizeof(size_t));
    b.strs     = (char **)   calloc(initial_capacity, sizeof(char *));
    b.size     = 0;
    b.capacity = initial_capacity;
    return b;
}

void destruct(bucket_t *b) {
    free(b->hashes); b->hashes = nullptr;
    free(b->lens);   b->lens   = nullptr;
    free(b->strs);   b->strs   = nullptr;
    b->size = b->capacity = 0;
}

static void grow(bucket_t *b) {
    size_t cap  = b->capacity * 2;
    b->hashes   = (uint64_t *)realloc(b->hashes, cap * sizeof(uint64_t));
    b->lens     = (size_t *)  realloc(b->lens,   cap * sizeof(size_t));
    b->strs     = (char **)   realloc(b->strs,   cap * sizeof(char *));
    b->capacity = cap;
}

void add(bucket_t *b, const char *str, size_t len, uint64_t hash) {
    if (b->size == b->capacity) grow(b);
    size_t i     = b->size++;
    b->hashes[i] = hash;
    b->lens[i]   = len;
    b->strs[i]   = (char *)str;  // borrowed pointer, ownership stays with caller
}

bool contains(const bucket_t *b, const char *str, size_t len, uint64_t hash) {
    const size_t n = b->size;
    if (!n) return false;

    const uint64_t * __restrict hashes = b->hashes;
    const size_t   * __restrict lens   = b->lens;
    char * const   * __restrict strs   = b->strs;

    size_t i = 0;

    // Сравниваем по 4 хеша за одну итерацию цикла
#ifdef __AVX2__
    const __m256i vtarget = _mm256_set1_epi64x((long long)hash);

    for (; i + 4 <= n; i += 4) {
        __m256i vh   = _mm256_loadu_si256((const __m256i *)(hashes + i));
        __m256i cmp  = _mm256_cmpeq_epi64(vh, vtarget);
        int     mask = _mm256_movemask_epi8(cmp);

        // каждая из 4 lanes даёт 8 бит в mask; mask==0 - все 4 хеша не совпали
        if (mask != 0) {
            for (int j = 0; j < 4; j++) {
                if ((mask >> (j * 8)) & 0xFF) {
                    if (lens[i + j] == len &&
                        memcmp(strs[i + j], str, len) == 0)
                        return true;
                }
            }
        }
    }
#endif
    for (; i < n; i++) {
        if (hashes[i] == hash && lens[i] == len &&
            memcmp(strs[i], str, len) == 0)
            return true;
    }
    return false;
}

} // namespace bucket_soa

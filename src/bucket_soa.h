#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

namespace bucket_soa {

struct bucket_t {
    uint64_t *hashes;   // SoA: hashes first - SIMD-сканирование идёт по этому массиву
    size_t   *lens;
    char    **strs;
    size_t    size;
    size_t    capacity;
};

bucket_t construct(size_t initial_capacity);
void     destruct(bucket_t *b);
bool     contains(const bucket_t *b, const char *str, size_t len, uint64_t hash);
void     add(bucket_t *b, const char *str, size_t len, uint64_t hash);

} // namespace bucket_soa

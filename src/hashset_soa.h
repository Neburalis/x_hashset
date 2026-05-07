#pragma once
#include "bucket_soa.h"
#include <stdint.h>

namespace hashset_soa {

struct hashset_t {
    bucket_soa::bucket_t *buckets;
    size_t                nbuckets;
};

hashset_t construct(size_t nbuckets);
void      destruct(hashset_t *hs);
bool      add(hashset_t *hs, const char *str, size_t len, uint64_t hash);
bool      contains(const hashset_t *hs, const char *str, size_t len, uint64_t hash);

} // namespace hashset_soa

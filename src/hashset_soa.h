#pragma once
#include "bucket_soa.h"
#include "stringNthong.h"

namespace hashset_soa {

struct hashset_t {
    bucket_soa::bucket_t *buckets;
    size_t                nbuckets;
    x_str::hash_fp        hash_fn;
};

hashset_t construct(x_str::hash_fp fn, size_t nbuckets);
void      destruct(hashset_t *hs);
bool      add(hashset_t *hs, x_str::x_str_t str);
bool      contains(const hashset_t *hs, x_str::x_str_t str);

} // namespace hashset_soa

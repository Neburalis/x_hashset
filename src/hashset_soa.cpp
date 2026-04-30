#include "hashset_soa.h"
#include <stdlib.h>

namespace hashset_soa {

hashset_t construct(x_str::hash_fp fn, size_t nbuckets) {
    hashset_t hs;
    hs.nbuckets = nbuckets;
    hs.hash_fn  = fn;
    hs.buckets  = (bucket_soa::bucket_t *)calloc(nbuckets, sizeof(bucket_soa::bucket_t));
    for (size_t i = 0; i < nbuckets; i++)
        hs.buckets[i] = bucket_soa::construct(4);
    return hs;
}

void destruct(hashset_t *hs) {
    for (size_t i = 0; i < hs->nbuckets; i++)
        bucket_soa::destruct(&hs->buckets[i]);
    free(hs->buckets);
    hs->buckets  = nullptr;
    hs->nbuckets = 0;
}

bool add(hashset_t *hs, x_str::x_str_t str) {
    uint64_t h    = (uint64_t)hs->hash_fn(&str);
    size_t   buck = h % hs->nbuckets;
    if (bucket_soa::contains(&hs->buckets[buck], str.str, str.len, h))
        return true;
    bucket_soa::add(&hs->buckets[buck], str.str, str.len, h);
    return true;
}

bool contains(const hashset_t *hs, x_str::x_str_t str) {
    uint64_t h    = (uint64_t)hs->hash_fn(&str);
    size_t   buck = h % hs->nbuckets;
    return bucket_soa::contains(&hs->buckets[buck], str.str, str.len, h);
}

} // namespace hashset_soa

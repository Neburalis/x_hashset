#include "hashset_soa.h"
#include <stdlib.h>

namespace hashset_soa {

hashset_t construct(size_t nbuckets) {
    hashset_t hs;
    hs.nbuckets = nbuckets;
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

bool add(hashset_t *hs, const char *str, size_t len, uint64_t hash) {
    size_t buck = hash % hs->nbuckets;
    if (bucket_soa::contains(&hs->buckets[buck], str, len, hash))
        return true;
    bucket_soa::add(&hs->buckets[buck], str, len, hash);
    return true;
}

bool contains(const hashset_t *hs, const char *str, size_t len, uint64_t hash) {
    size_t buck = hash % hs->nbuckets;
    return bucket_soa::contains(&hs->buckets[buck], str, len, hash);
}

} // namespace hashset_soa

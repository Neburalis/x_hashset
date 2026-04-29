#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashset.h"

namespace x_hashset {

hashset_t construct(x_str::hash_fp hash_fn, size_t size) {
    hashset_t res = {
        .size    = size,
        .lists   = 0,
        .hash_fn = hash_fn
    };

    x_list::list_t **lists = (x_list::list_t **) calloc(size, sizeof(x_list::list_t *));

    for (size_t i = 0; i < size; ++i) {
        lists[i] = x_list::constructor(20);
    }

    res.lists = lists;

    return res;
}

void destruct(hashset_t *hashset) {
    for (size_t i = 0; i < hashset->size; ++i) {
        x_list::destructor(&(hashset->lists[i]));
    }
    free(hashset->lists);

    hashset->size    = 0;
    hashset->lists   = 0;
    hashset->hash_fn = 0;

    hashset = 0;
}

bool verifier(hashset_t hashset) {
    bool res = true;
    for (size_t i = 0; i < hashset.size; ++i) {
        res &= x_list::verifier(hashset.lists[i]);
    }
    return res;
}

void dump(hashset_t hashset, FILE *fp) {
    for (size_t i = 0; i < hashset.size; ++i) {
        char *str = x_list__dump_to_str(hashset.lists[i], 0);
#pragma diagnostic push -Wformat-security
        fprintf(fp, str);
#pragma diagnostic pop -Wformat-security
        free(str);
    }
}

bool add(hashset_t hashset, x_str::x_str_t str) {
    unsigned long h = (str.hash_fn == hashset.hash_fn)
                      ? str.hash
                      : (str.hash_fn = hashset.hash_fn, hashset.hash_fn(&str));
    unsigned long bucket = h % hashset.size;

    size_t found = x_list::slow::search(hashset.lists[bucket], str);
    if (found) return true;

    size_t idx = x_list::push_front(hashset.lists[bucket], str);
    if (!idx && hashset.lists[bucket]->errno == x_list::LIST_OVERFLOW) {
        x_list::slow::resize(hashset.lists[bucket],
                             hashset.lists[bucket]->capacity * 2);
        found = x_list::slow::search(hashset.lists[bucket], str);
        if (!found) idx = x_list::push_front(hashset.lists[bucket], str);
    }
    return idx != 0 || found != 0;
}

bool contains(hashset_t hashset, x_str::x_str_t str) {
    unsigned long h = (str.hash_fn == hashset.hash_fn)
                      ? str.hash
                      : hashset.hash_fn(&str);

    return x_list::slow::search(hashset.lists[h % hashset.size], str);
}

void dump_hist(hashset_t hashset, const char *csv_path) {
    FILE *fp = fopen(csv_path, "w");
    if (!fp) {
        fprintf(stderr, "dump_hist: cannot open %s\n", csv_path);
        return;
    }

    fprintf(fp, "bucket,count\n");
    for (size_t i = 0; i < hashset.size; ++i)
        fprintf(fp, "%zu,%zu\n", i, x_list::size(hashset.lists[i]));

    fclose(fp);
}

} // namespace x_hashset

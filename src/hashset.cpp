#include <stdlib.h>

#include "hashset.h"

using namespace x_hashset;

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
        char *str = dump_to_str(hashset.lists[i], 0);
        fprintf(fp, str);
        free(str);
    }
}

bool add(hashset_t hashset, x_str::x_str_t str) {
    unsigned long hash = str.hash ?
        str.hash :
        (str.hash_fn == hashset.hash_fn ?
            str.hash = str.hash_fn(&str), str.hash :
            str.hash_fn = hashset.hash_fn, str.hash = str.hash_fn(&str), str.hash
        );
    if (hash == 0) return false;
    hash = (hash - 1) % hashset.size;

    size_t var = x_list::slow::search(hashset.lists[hash], str);
    return var ? var : x_list::push_front(hashset.lists[hash], str);
}

bool contains(hashset_t hashset, x_str::x_str_t str) {
    unsigned long hash = str.hash ?
        str.hash :
        (str.hash_fn == hashset.hash_fn ?
            str.hash = str.hash_fn(&str), str.hash :
            str.hash_fn = hashset.hash_fn, str.hash = str.hash_fn(&str), str.hash
        );
    if (hash == 0) return false;
    hash = (hash - 1) % hashset.size;

    return x_list::slow::search(hashset.lists[hash], str);
}

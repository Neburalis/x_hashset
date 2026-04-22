#ifndef HASHSET_H
#define HASHSET_H

#include <stddef.h>
#include "external/x_list/src/list.h"
#include "external/string_and_thong/stringNthong.h"

namespace x_hashset {

typedef struct {
    size_t           size;
    x_list::list_t **lists;    // array of lists
    x_str::hash_fp   hash_fn;

} hashset_t;

hashset_t x_hashset::construct(x_str::hash_fp hash_fn, size_t size);

void x_hashset::destruct(hashset_t *hashset);

bool x_hashset::verifier(hashset_t *hashset);

void x_hashset::dump(hashset_t hashset);

bool x_hashset::add(hashset_t hashset, x_str::x_str_t str);

bool x_hashset::contains(hashset_t hashset, x_str::x_str_t str);

}

#endif // HASHSET_H

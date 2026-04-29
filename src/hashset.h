#ifndef HASHSET_H
#define HASHSET_H

#include <stddef.h>
#include "list.h"
#include "stringNthong.h"

namespace x_hashset {

typedef struct {
    size_t           size;
    x_list::list_t **lists;    // array of lists
    x_str::hash_fp   hash_fn;

} hashset_t;

hashset_t construct(x_str::hash_fp hash_fn, size_t size);

void destruct(hashset_t *hashset);

bool verifier(hashset_t hashset);

void dump(hashset_t hashset, FILE *fp);

// Write per-bucket element counts to <csv_path> (columns: bucket,count).
void dump_hist(hashset_t hashset, const char *csv_path);

bool add(hashset_t hashset, x_str::x_str_t str);

bool contains(hashset_t hashset, x_str::x_str_t str);

}

#endif // HASHSET_H

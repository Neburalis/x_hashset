#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "hashset.h"
#include "hashfuncs.h"

struct hash_func_entry {
    x_str::hash_fp  fp;
    const char     *name;
    size_t          size;
};

const char * remove_last_terminal_str = "\r\033[2K";

#define HASH_ENTRY(f, sz) {hash::f, #f, (sz)}
static hash_func_entry all_funcs[] = {
    HASH_ENTRY(why_not,     501),
    HASH_ENTRY(first_char,  197),
    HASH_ENTRY(first_char,  501),
    HASH_ENTRY(len,         97),
    HASH_ENTRY(len,         501),
    HASH_ENTRY(sum,         501),
    HASH_ENTRY(sum,         5003),
    HASH_ENTRY(rol,         501),
    HASH_ENTRY(rol,         5003),
    HASH_ENTRY(ror,         501),
    HASH_ENTRY(ror,         5003),
    HASH_ENTRY(crc32,       501),
    HASH_ENTRY(crc32,       5003),
    // HASH_ENTRY(crc32_hw,         501),
    // HASH_ENTRY(crc32_hw,         5003),
    // HASH_ENTRY(crc32_hw_aligned, 501),
    // HASH_ENTRY(crc32_hw_aligned, 5003),
};
#undef HASH_ENTRY

static const int N_FUNCS = (int)(sizeof(all_funcs) / sizeof(all_funcs[0]));

using namespace x_hashset;

// each str must be freed by caller.
static size_t load_words(const char *path, x_str::x_str_t **out) {
    FILE *f = fopen(path, "r");
    if (!f) {
        perror(path);
        *out = nullptr;
        return 0;
    }

    size_t cap = 4096, n = 0;
    *out = (x_str::x_str_t *) calloc(cap, sizeof(x_str::x_str_t));
    if (!*out) {
        fclose(f);
        return 0;
    }

    char buf[512];
    while (fgets(buf, sizeof(buf), f)) {
        size_t len = strlen(buf);
        if (len && buf[len-1] == '\n') buf[--len] = '\0';
        if (!len) continue;
        if (n == cap) {
            cap *= 2;
            x_str::x_str_t *tmp = (x_str::x_str_t *) realloc(*out, cap * sizeof(x_str::x_str_t));
            if (!tmp) break;
            *out = tmp;
        }
        (*out)[n++] = x_str::construct(strdup(buf));
    }
    fclose(f);
    return n;
}

static void free_words(x_str::x_str_t *words, size_t n) {
    for (size_t i = 0; i < n; i++)
        free(words[i].str);
    free(words);
}

void test_bucket_distribution(size_t n_train, x_str::x_str_t *train) {
    printf("=== Part 1: bucket distribution data ===\n\n");

    for (int fi = 0; fi < N_FUNCS; fi++) {
        const char *name = all_funcs[fi].name;
        size_t      size = all_funcs[fi].size;
        printf("  Computing %s ...", name);
        fflush(stdout);
        hashset_t hs = construct(all_funcs[fi].fp, size);
        for (size_t i = 0; i < n_train; i++)
            add(hs, train[i]);

        char csv[256];
        snprintf(csv, sizeof(csv), "data/hist_%s_%zu.csv", name, size);
        dump_hist(hs, csv);
        destruct(&hs);
        printf("\r\033[2K  %-12s  %6zu buckets  ->  %s\n", name, size, csv);
    }
}

int main() {
    x_str::x_str_t *train = nullptr;
    size_t n_train = load_words("texts/words.txt", &train);
    printf("Training words: %zu  (texts/words.txt)\n\n", n_train);

    test_bucket_distribution(n_train, train);
    printf("\nDone. Run:  python3 scripts/analyze.py\n");

    free_words(train, n_train);
    return 0;
}

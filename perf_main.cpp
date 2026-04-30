#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashset.h"
#include "hashfuncs.h"

static const size_t BUCKETS  = 997;
static const int    REPS     = 100;

static size_t load_words(const char *path, x_str::x_str_t **out) {
    FILE *f = fopen(path, "r");
    if (!f) { perror(path); *out = nullptr; return 0; }

    size_t cap = 4096, n = 0;
    *out = (x_str::x_str_t *) calloc(cap, sizeof(x_str::x_str_t));
    if (!*out) { fclose(f); return 0; }

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

int main() {
    x_str::x_str_t *train = nullptr;
    size_t n_train = load_words("texts/words.txt", &train);
    printf("Loaded %zu training words\n", n_train);

    x_str::x_str_t *test = nullptr;
    size_t n_test = load_words("texts/test_words.txt", &test);
    printf("Loaded %zu test words\n", n_test);

    x_hashset::hashset_t hs = x_hashset::construct(hash::crc32_hw, BUCKETS);
    for (size_t i = 0; i < n_train; i++)
        x_hashset::add(hs, train[i]);

    printf("Running contains x%d ...\n", REPS);
    volatile bool sink = false;
    for (int r = 0; r < REPS; r++)
        for (size_t i = 0; i < n_test; i++)
            sink = x_hashset::contains(hs, test[i]);

    (void)sink;
    x_hashset::destruct(&hs);

    for (size_t i = 0; i < n_train; i++) free(train[i].str);
    free(train);
    for (size_t i = 0; i < n_test; i++) free(test[i].str);
    free(test);
    return 0;
}

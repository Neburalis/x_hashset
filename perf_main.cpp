#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include "hashset.h"
#include "hashfuncs.h"

static const size_t BUCKETS    = 997;
static const int    REPS       = 100;
static const size_t TEST_LIMIT = 100000;

static size_t load_words(const char *path, x_str::x_str_t **out) {
    FILE *f = fopen(path, "r");
    if (!f) { perror(path); *out = nullptr; return 0; }

    size_t cap = 4096, n = 0;
    *out = (x_str::x_str_t *)calloc(cap, sizeof(x_str::x_str_t));
    if (!*out) { fclose(f); return 0; }

    char buf[512];
    while (fgets(buf, sizeof(buf), f)) {
        size_t len = strlen(buf);
        if (len && buf[len-1] == '\n') buf[--len] = '\0';
        if (!len) continue;
        if (n == cap) {
            cap *= 2;
            x_str::x_str_t *tmp = (x_str::x_str_t *)realloc(*out, cap * sizeof(x_str::x_str_t));
            if (!tmp) break;
            *out = tmp;
        }
        (*out)[n++] = x_str::construct(strdup(buf));
    }
    fclose(f);
    return n;
}

static void shuffle(size_t *idx, size_t n) {
    for (size_t i = 0; i < n; i++) idx[i] = i;
    for (size_t i = n - 1; i > 0; i--) {
        size_t j = (size_t)rand() % (i + 1);
        size_t tmp = idx[i]; idx[i] = idx[j]; idx[j] = tmp;
    }
}

int main() {
    x_str::x_str_t *train = nullptr;
    size_t n_train = load_words("texts/words.txt", &train);
    printf("Loaded %zu training words\n", n_train);

    x_str::x_str_t *test = nullptr;
    size_t n_test_all = load_words("texts/test_words.txt", &test);
    size_t n_test = n_test_all < TEST_LIMIT ? n_test_all : TEST_LIMIT;
    printf("Loaded %zu test words (of %zu)\n", n_test, n_test_all);

    x_hashset::hashset_t hs = x_hashset::construct(hash::crc32, BUCKETS);
    for (size_t i = 0; i < n_train; i++)
        x_hashset::add(hs, train[i]);

    size_t *idx = (size_t *)malloc(n_test * sizeof(size_t));
    srand(42);
    shuffle(idx, n_test);

#ifdef NDEBUG
    const char *variant = "base";
#else
    const char *variant = "base_verify";
#endif
    char csv_path[256];
    snprintf(csv_path, sizeof(csv_path), "data/wall_%s.csv", variant);
    FILE *csv_fp = fopen(csv_path, "w");
    if (csv_fp) fprintf(csv_fp, "rep,rep_ms\n");

    printf("Running %d reps x %zu words ...\n", REPS, n_test);
    volatile bool sink = false;
    for (int r = 0; r < REPS; r++) {
        struct timespec w0, w1;
        clock_gettime(CLOCK_MONOTONIC, &w0);
        for (size_t i = 0; i < n_test; i++)
            sink = x_hashset::contains(hs, test[idx[i]]);
        clock_gettime(CLOCK_MONOTONIC, &w1);
        uint64_t ns = (uint64_t)(w1.tv_sec - w0.tv_sec) * 1000000000ULL
                    + (uint64_t)(w1.tv_nsec - w0.tv_nsec);
        printf("  rep %2d: %6.1f ms\n", r, ns / 1e6);
        if (csv_fp) fprintf(csv_fp, "%d,%.3f\n", r, ns / 1e6);
        shuffle(idx, n_test);
    }
    (void)sink;
    if (csv_fp) fclose(csv_fp);

    free(idx);
    x_hashset::destruct(&hs);
    for (size_t i = 0; i < n_train; i++) free(train[i].str);
    free(train);
    for (size_t i = 0; i < n_test_all; i++) free(test[i].str);
    free(test);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include "hashset_soa.h"
#include "hashfuncs.h"

static const size_t BUCKETS    = 997;
static const int    REPS       = 100;
static const size_t TEST_LIMIT = 100000;

struct words_t {
    char    **strs;
    size_t   *lens;
    uint64_t *hashes;
    size_t    n;
    size_t    cap;
};

static words_t load_words(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return {}; }

    words_t w;
    w.n   = 0;
    w.cap = 4096;
    w.strs   = (char **)  malloc(w.cap * sizeof(char *));
    w.lens   = (size_t *) malloc(w.cap * sizeof(size_t));
    w.hashes = (uint64_t *)malloc(w.cap * sizeof(uint64_t));

    char buf[512];
    while (fgets(buf, sizeof(buf), f)) {
        size_t len = strlen(buf);
        if (len && buf[len-1] == '\n') buf[--len] = '\0';
        if (!len) continue;
        if (w.n == w.cap) {
            w.cap *= 2;
            w.strs   = (char **)  realloc(w.strs,   w.cap * sizeof(char *));
            w.lens   = (size_t *) realloc(w.lens,   w.cap * sizeof(size_t));
            w.hashes = (uint64_t *)realloc(w.hashes, w.cap * sizeof(uint64_t));
        }
        char *s = strdup(buf);
#ifdef USE_HW_CRC32
        w.hashes[w.n] = hash::crc32_hw_raw(s, len);
#else
        w.hashes[w.n] = hash::crc32_sw_raw(s, len);
#endif
        w.strs[w.n] = s;
        w.lens[w.n] = len;
        w.n++;
    }
    fclose(f);
    return w;
}

static void words_free(words_t *w) {
    for (size_t i = 0; i < w->n; i++) free(w->strs[i]);
    free(w->strs);
    free(w->lens);
    free(w->hashes);
    w->strs = nullptr; w->lens = nullptr; w->hashes = nullptr;
    w->n = w->cap = 0;
}

static void shuffle(size_t *idx, size_t n) {
    for (size_t i = 0; i < n; i++) idx[i] = i;
    for (size_t i = n - 1; i > 0; i--) {
        size_t j = (size_t)rand() % (i + 1);
        size_t tmp = idx[i]; idx[i] = idx[j]; idx[j] = tmp;
    }
}

int main() {
    words_t train = load_words("texts/words.txt");
    printf("Loaded %zu training words\n", train.n);

    words_t test = load_words("texts/test_words.txt");
    size_t n_test = test.n < TEST_LIMIT ? test.n : TEST_LIMIT;
    printf("Loaded %zu test words (of %zu)\n", n_test, test.n);

    hashset_soa::hashset_t hs = hashset_soa::construct(BUCKETS);
    for (size_t i = 0; i < train.n; i++)
        hashset_soa::add(&hs, train.strs[i], train.lens[i], train.hashes[i]);

    size_t *idx = (size_t *)malloc(n_test * sizeof(size_t));
    srand(42);
    shuffle(idx, n_test);

#ifdef USE_HW_CRC32
    const char *variant = "soa_hw";
    printf("Hash: CRC32 hardware (loop)\n");
#else
    const char *variant = "soa";
    printf("Hash: CRC32 software\n");
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
            sink = hashset_soa::contains(&hs,
                       test.strs[idx[i]], test.lens[idx[i]], test.hashes[idx[i]]);
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
    hashset_soa::destruct(&hs);
    words_free(&train);
    words_free(&test);
    return 0;
}

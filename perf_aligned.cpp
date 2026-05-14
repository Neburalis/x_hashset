#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "hashset_soa.h"
#include "hashfuncs.h"
#include "simd_str.h"
#include "aligned_store.h"
#include "io_utils.h"

static const size_t BUCKETS    = 997;
static const int    REPS       = 100;
static const size_t TEST_LIMIT = 100000;

struct word_data_t {
    aligned_store_t store;
    size_t         *lens;
    uint64_t       *hashes;
    size_t          n;
};

static word_data_t load_words(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return {}; }

    char buf[256];
    size_t count = lines_in_file(f);

    word_data_t w;
    w.n      = 0;
    w.store  = aligned_store_create(count);
    w.lens   = (size_t *)  calloc(count, sizeof(size_t));
    w.hashes = (uint64_t *)calloc(count, sizeof(uint64_t));

    while (fgets(buf, sizeof(buf), f)) {
        size_t len = simd_strlen(buf);
        if (len && buf[len - 1] == '\n') buf[--len] = '\0';
        if (!len || len >= ALIGNED_SLOT) continue;
        const char *slot = aligned_store_intern(&w.store, buf, len);
        w.hashes[w.n] = hash::crc32_hw_aligned32(slot);
        w.lens[w.n]   = len;
        w.n++;
    }
    fclose(f);
    return w;
}

static void word_data_free(word_data_t *w) {
    aligned_store_destroy(&w->store);
    free(w->lens);
    free(w->hashes);
    w->lens = w->hashes = nullptr;
    w->n = 0;
}

static void shuffle(size_t *idx, size_t n) {
    for (size_t i = 0; i < n; i++) idx[i] = i;
    for (size_t i = n - 1; i > 0; i--) {
        size_t j = (size_t)rand() % (i + 1);
        size_t tmp = idx[i]; idx[i] = idx[j]; idx[j] = tmp;
    }
}

int main() {
    word_data_t train = load_words("texts/words.txt");
    printf("Loaded %zu training words\n", train.n);

    word_data_t test = load_words("texts/test_words.txt");
    size_t n_test = test.n < TEST_LIMIT ? test.n : TEST_LIMIT;
    printf("Loaded %zu test words (of %zu)\n", n_test, test.n);

    hashset_soa::hashset_t hs = hashset_soa::construct(BUCKETS);
    for (size_t i = 0; i < train.n; i++)
        hashset_soa::add(&hs, aligned_store_get(&train.store, i),
                         train.lens[i], train.hashes[i]);

    size_t *idx = (size_t *)malloc(n_test * sizeof(size_t));
    srand(42);
    shuffle(idx, n_test);

    FILE *csv_fp = fopen("data/wall_aligned.csv", "w");
    if (csv_fp) fprintf(csv_fp, "rep,rep_ms\n");

    printf("Hash: CRC32 hardware unrolled (no loop, no len)\n");
    printf("Running %d reps x %zu words ...\n", REPS, n_test);

    volatile bool sink = false;
    for (int r = 0; r < REPS; r++) {
        struct timespec w0, w1;
        clock_gettime(CLOCK_MONOTONIC, &w0);
        for (size_t i = 0; i < n_test; i++)
            sink = hashset_soa::contains(&hs,
                       aligned_store_get(&test.store, idx[i]),
                       test.lens[idx[i]], test.hashes[idx[i]]);
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
    word_data_free(&train);
    word_data_free(&test);
    return 0;
}

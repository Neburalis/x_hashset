#include <stdio.h>
#include <stdlib.h>

#include "hashset_soa.h"
#include "hashfuncs.h"
#include "simd_str.h"
#include "aligned_store.h"
#include "io_utils.h"

static const size_t BUCKETS = 997;
static const int    REPS    = 100;

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
        w.lens[w.n]   = len;
        w.hashes[w.n] = hash::crc32_hw_raw(slot, len);
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

int main() {
    word_data_t train = load_words("texts/words.txt");
    printf("Loaded %zu training words\n", train.n);

    word_data_t test = load_words("texts/test_words.txt");
    printf("Loaded %zu test words\n", test.n);

    hashset_soa::hashset_t hs = hashset_soa::construct(BUCKETS);
    for (size_t i = 0; i < train.n; i++)
        hashset_soa::add(&hs, aligned_store_get(&train.store, i),
                         train.lens[i], train.hashes[i]);

    printf("Running contains x%d ...\n", REPS);
    volatile bool sink = false;
    for (int r = 0; r < REPS; r++)
        for (size_t i = 0; i < test.n; i++)
            sink = hashset_soa::contains(&hs, aligned_store_get(&test.store, i),
                                         test.lens[i], test.hashes[i]);

    (void)sink;
    hashset_soa::destruct(&hs);
    word_data_free(&train);
    word_data_free(&test);
    return 0;
}

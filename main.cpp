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

#define HASH_ENTRY(f, sz) {hash::f, #f, (sz)}
static hash_func_entry all_funcs[] = {
    HASH_ENTRY(why_not,     997),
    HASH_ENTRY(first_char,  197),
    HASH_ENTRY(first_char,  997),
    HASH_ENTRY(len,         97),
    HASH_ENTRY(len,         997),
    HASH_ENTRY(sum,         997),
    HASH_ENTRY(sum,         5003),
    HASH_ENTRY(rol,         997),
    HASH_ENTRY(ror,         997),
    HASH_ENTRY(crc32,       997),
    HASH_ENTRY(crc32,       5003),
    HASH_ENTRY(crc32_hw,    997),
    HASH_ENTRY(crc32_hw,    5003),
};
#undef HASH_ENTRY

static const int    N_FUNCS         = (int)(sizeof(all_funcs) / sizeof(all_funcs[0]));
static const size_t PERF_TEST_LIMIT = 100000;
static const int    PERF_REPS       = 5;

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

static inline uint64_t rdtsc_start() {
    unsigned lo, hi;
    __asm__ volatile ("cpuid\n\t"
                      "rdtsc\n\t"
                      : "=a"(lo), "=d"(hi)
                      :
                      : "%rbx", "%rcx");
    return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t rdtsc_end() {
    unsigned lo, hi;
    __asm__ volatile ("rdtscp\n\t"
                      "mov %%edx, %0\n\t"
                      "mov %%eax, %1\n\t"
                      "cpuid\n\t"
                      : "=r"(hi), "=r"(lo)
                      :
                      : "%rax", "%rbx", "%rcx", "%rdx");
    return ((uint64_t)hi << 32) | lo;
}

int main() {
    x_str::x_str_t *train = nullptr;
    size_t n_train = load_words("texts/words.txt", &train);
    printf("Training words: %zu  (texts/words.txt - War and Peace)\n", n_train);

    x_str::x_str_t *test_all = nullptr;
    size_t n_test_all = load_words("texts/test_words.txt", &test_all);
    size_t n_test = (n_test_all < PERF_TEST_LIMIT) ? n_test_all : PERF_TEST_LIMIT;
    printf("Test words:     %zu of %zu  (texts/test_words.txt - /usr/share/dict/words)\n\n",
           n_test, n_test_all);

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

    /* ---- Part 2: search timing ---- */
    if (!n_test) {
        printf("\nNo test words loaded; skipping timing.\n");
        free_words(train, n_train);
        return 0;
    }

    printf("\n=== Saving search timing data (%zu words x %d reps) ===\n\n",
           n_test, PERF_REPS);

    FILE *timing_fp = fopen("data/timing.csv", "w");
    if (!timing_fp) {
        perror("data/timing.csv");
        free_words(train, n_train);
        free_words(test_all, n_test_all);
        return 1;
    }
    fprintf(timing_fp, "func,size,dt\n");

    for (int fi = 0; fi < N_FUNCS; fi++) {
        const char *name = all_funcs[fi].name;
        size_t      size = all_funcs[fi].size;
        printf("  Computing %s ...", name);
        fflush(stdout);
        hashset_t hs = construct(all_funcs[fi].fp, size);
        for (size_t i = 0; i < n_train; i++)
            add(hs, train[i]);

        for (int r = 0; r < PERF_REPS; r++) {
            for (size_t i = 0; i < n_test; i++) {
                uint64_t t0 = rdtsc_start();
                contains(hs, test_all[i]);
                uint64_t t1 = rdtsc_end();
                fprintf(timing_fp, "%s,%zu,%llu\n",
                        name, size, (unsigned long long)(t1 - t0));
            }
        }

        destruct(&hs);
        printf("\r\033[2K  %-12s  %6zu buckets  done\n", name, size);
    }

    fclose(timing_fp);
    printf("\nAll data saved. Run:  python3 scripts/analyze.py\n");

    free_words(train, n_train);
    free_words(test_all, n_test_all);
    return 0;
}

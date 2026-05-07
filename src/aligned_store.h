#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static const size_t ALIGNED_SLOT = 32;

struct aligned_store_t {
    char  *buf;
    size_t n_slots;
    size_t used;   // number of slots occupied
};

static inline aligned_store_t aligned_store_create(size_t n_slots) {
    aligned_store_t s;
    s.buf     = (char *)aligned_alloc(ALIGNED_SLOT, n_slots * ALIGNED_SLOT);
    s.n_slots = n_slots;
    s.used    = 0;
    memset(s.buf, 0, n_slots * ALIGNED_SLOT);
    return s;
}

static inline void aligned_store_destroy(aligned_store_t *s) {
    free(s->buf);
    s->buf = nullptr;
    s->n_slots = s->used = 0;
}

static inline char *aligned_store_intern(aligned_store_t *s,
                                         const char *str, size_t len) {
    char *slot = s->buf + s->used * ALIGNED_SLOT;
    memcpy(slot, str, len + 1);   // str + null; rest already zero
    s->used++;
    return slot;
}

static inline const char *aligned_store_get(const aligned_store_t *s, size_t i) {
    return s->buf + i * ALIGNED_SLOT;
}

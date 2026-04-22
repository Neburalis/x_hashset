#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

ssize_t count_needle_in_haystack(char * haystack, const size_t haystack_len, const char needle) {
    assert(haystack != NULL     && "U must pass haystack to count needles");
    assert(needle   != '\0'       && "U must pass needle other than '\\0'");

    if (haystack == NULL) {
        errno = EINVAL;
        // ERROR_MSG("U must pass haystack to count needles");
        return -1;
    }

    if (needle == '\0') {
        errno = EINVAL;
        // ERROR_MSG("U must pass needle other than '\\0'");
        return -1;
    }

    ssize_t count = 0;
    char * endptr = haystack + haystack_len;
    while (haystack != NULL && haystack < endptr) {
        haystack = strchr(haystack, needle);
        if (haystack != NULL) {
            ++count;
            ++haystack;
        }
    }
    return count; // Дошли до haystack_len но не встретили '\0'
}

ssize_t replace_needle_in_haystack
    (char * haystack, const size_t haystack_len, const char src, const char dst) {
    assert(haystack != NULL         && "U must pass haystack to count needles");
    assert(src      != '\0'         && "U must pass src other than '\\0'");

    if (haystack == 0) {
        errno = EINVAL;
        // ERROR_MSG("U must pass haystack to count needles");
        return -1;
    }

    if (src == '\0') {
        errno = EINVAL;
        // ERROR_MSG("U must pass src other than '\\0'");
        return -1;
    }

    ssize_t count = 0;
    char * endptr = haystack + haystack_len;
    while (haystack != NULL && haystack < endptr) {
        haystack = strchr(haystack, src);
        if (haystack != NULL) {
            ++count;
            *haystack = dst;
            ++haystack;
        }
    }
    return count; // Дошли до haystack_len но не встретили '\0'
}

int is_not_empty(const char *str) {
    return str != NULL && str[0] != '\0';
}

void move_ptr_to_first_alpha_symbol(const char ** ptr, int backword) {
    assert(ptr != NULL);
    assert(*ptr != NULL);

    while (**ptr != '\0' && !isalpha(**ptr)) {
        if (backword)
            --(*ptr);
        else // forward
            ++(*ptr);
    }
}

void move_ptr_to_first_not_space_symbol(const char ** ptr, int backword) {
    assert(ptr != NULL);
    assert(*ptr != NULL);

    while (**ptr != '\0' && isspace(**ptr)) {
        if (backword)
            --(*ptr);
        else // forward
            ++(*ptr);
    }
}

/*
this algorithm (k=33) was first reported by dan bernstein many years ago in comp.lang.c.
another version of this algorithm (now favored by bernstein) uses xor:
hash(i) = hash(i - 1) * 33 ^ str[i];
the magic of number 33 (why it works better than many other constants, prime or not)
has never been adequately explained.
*/
unsigned long djb2(const unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++) != '\0')
        hash = ((hash << 5) + hash) + (unsigned long long) c; /* hash * 33 + c */

    return hash;
}

/*
this algorithm was created for sdbm (a public-domain reimplementation of ndbm) database library.
it was found to do well in scrambling bits, causing better distribution of the keys and fewer splits.
it also happens to be a good general hashing function with good distribution.
the actual function is hash(i) = hash(i - 1) * 65599 + str[i];
what is included below is the faster version used in gawk.
[there is even a faster, duff-device version]
the magic prime constant 65599 (2^6 + 2^16 - 1) was picked out of thin air
while experimenting with many different constants. this is one of the algorithms
used in berkeley db (see sleepycat) and elsewhere.
*/
unsigned long sdbm(const char * str) {
    unsigned long hash = 0;
    int c;

    while ((c = *str++) != '\0')
        hash = (unsigned long long) c + (hash << 16) + (hash << 6) - hash;

    return hash;
}
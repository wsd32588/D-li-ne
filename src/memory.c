#include "memory.h"

#include <stdlib.h>
#include <string.h>

void *dline_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    void *ptr = malloc(size);
    if (ptr == NULL) {
        return NULL;
    }

    return ptr;
}

void *dline_realloc(void *pointer, size_t size) {
    if (size == 0) {
        free(pointer);
        return NULL;
    }

    return realloc(pointer, size);
}

void dline_free(void *pointer) {
    free(pointer);
}

char *dline_strdup(const char *s) {
    if (s == NULL) {
        return NULL;
    }

    size_t length = strlen(s) + 1;
    char *copy = dline_malloc(length);

    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, s, length);
    return copy;
}

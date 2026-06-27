#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void *dline_malloc(size_t size);
void *dline_realloc(void *pointer, size_t size);
void dline_free(void *pointer);
char *dline_strdup(const char *s);

#endif

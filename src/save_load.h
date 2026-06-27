#ifndef DLINE_SAVE_LOAD_H
#define DLINE_SAVE_LOAD_H

#include <stddef.h>

#include "Dline/core.h"
#include "buffer.h"

typedef struct {
    size_t maximum_file_bytes;
} SaveLoadOptions;

SaveLoadOptions save_load_default_options(void);

/* The result buffer must be initialized. It is unchanged on failure. */
DlineResult save_load_read_file(
    const char *path,
    const SaveLoadOptions *options,
    DlineBuffer *result
);

#endif

#ifndef DLINE_BUFFER_H
#define DLINE_BUFFER_H

#include <stddef.h>
#include <stdint.h>

#include "Dline/core.h"

typedef struct {
    uint8_t *data;
    size_t length;
    size_t capacity;
} DlineBuffer;

DlineResult buffer_init(DlineBuffer *buffer);
void buffer_destroy(DlineBuffer *buffer);
DlineResult buffer_reserve(
    DlineBuffer *buffer,
    size_t required_capacity
);
DlineResult buffer_reserve_limited(
    DlineBuffer *buffer,
    size_t required_capacity,
    size_t maximum_capacity
);
DlineResult buffer_append(
    DlineBuffer *buffer,
    const uint8_t *data,
    size_t length
);
DlineResult buffer_append_limited(
    DlineBuffer *buffer,
    const uint8_t *data,
    size_t length,
    size_t maximum_capacity
);

#endif

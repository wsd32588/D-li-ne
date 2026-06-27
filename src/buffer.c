#include "buffer.h"

#include "memory.h"

#include <string.h>

enum {
    BUFFER_INITIAL_CAPACITY = 256
};

DlineResult buffer_init(DlineBuffer *buffer) {
    if (buffer == NULL) {
        return DLINE_ERROR_INVALID_ARGUMENT;
    }

    *buffer = (DlineBuffer){0};

    buffer->data = dline_malloc(BUFFER_INITIAL_CAPACITY);
    if (buffer->data == NULL) {
        return DLINE_ERROR_OUT_OF_MEMORY;
    }

    buffer->capacity = BUFFER_INITIAL_CAPACITY;
    return DLINE_OK;
}

void buffer_destroy(DlineBuffer *buffer) {
    if (buffer == NULL) {
        return;
    }

    dline_free(buffer->data);
    *buffer = (DlineBuffer){0};
}

DlineResult buffer_reserve(
    DlineBuffer *buffer,
    size_t required_capacity
) {
    if (buffer == NULL) {
        return DLINE_ERROR_INVALID_ARGUMENT;
    }

    if (required_capacity <= buffer->capacity) {
        return DLINE_OK;
    }

    size_t new_capacity = buffer->capacity;
    if (new_capacity == 0) {
        new_capacity = BUFFER_INITIAL_CAPACITY;
    }

    while (new_capacity < required_capacity) {
        if (new_capacity > SIZE_MAX / 2) {
            new_capacity = required_capacity;
            break;
        }

        new_capacity *= 2;
    }

    uint8_t *new_data = dline_realloc(buffer->data, new_capacity);
    if (new_data == NULL) {
        return DLINE_ERROR_OUT_OF_MEMORY;
    }

    buffer->data = new_data;
    buffer->capacity = new_capacity;
    return DLINE_OK;
}

DlineResult buffer_append(
    DlineBuffer *buffer,
    const uint8_t *data,
    size_t length
) {
    if (buffer == NULL || (data == NULL && length != 0)) {
        return DLINE_ERROR_INVALID_ARGUMENT;
    }

    if (length > SIZE_MAX - buffer->length) {
        return DLINE_ERROR_CAPACITY_OVERFLOW;
    }

    size_t required_capacity = buffer->length + length;
    DlineResult result = buffer_reserve(buffer, required_capacity);

    if (result != DLINE_OK) {
        return result;
    }

    if (length != 0) {
        memcpy(buffer->data + buffer->length, data, length);
        buffer->length += length;
    }

    return DLINE_OK;
}

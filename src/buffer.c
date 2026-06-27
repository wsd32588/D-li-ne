#include "buffer.h"

#include "limit.h"
#include "memory.h"

#include <string.h>

static const size_t BUFFER_INITIAL_CAPACITY = 256;

DlineResult buffer_init(DlineBuffer *buffer) {
    if (buffer == NULL) {
        return DLINE_ERROR_INVALID_ARGUMENT;
    }

    *buffer = (DlineBuffer){0};
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
    return buffer_reserve_limited(buffer, required_capacity, SIZE_MAX);
}

DlineResult buffer_reserve_limited(
    DlineBuffer *buffer,
    size_t required_capacity,
    size_t maximum_capacity
) {
    if (buffer == NULL) {
        return DLINE_ERROR_INVALID_ARGUMENT;
    }

    if (required_capacity > maximum_capacity) {
        return DLINE_ERROR_CAPACITY_OVERFLOW;
    }

    if (required_capacity <= buffer->capacity) {
        return DLINE_OK;
    }

    size_t new_capacity = 0;
    if (!limit_calculate_capacity(
            buffer->capacity,
            required_capacity,
            BUFFER_INITIAL_CAPACITY,
            maximum_capacity,
            &new_capacity
        )) {
        return DLINE_ERROR_CAPACITY_OVERFLOW;
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
    return buffer_append_limited(buffer, data, length, SIZE_MAX);
}

DlineResult buffer_append_limited(
    DlineBuffer *buffer,
    const uint8_t *data,
    size_t length,
    size_t maximum_capacity
) {
    if (buffer == NULL || (data == NULL && length != 0)) {
        return DLINE_ERROR_INVALID_ARGUMENT;
    }

    size_t required_capacity = 0;
    if (!limit_checked_add(
            buffer->length,
            length,
            maximum_capacity,
            &required_capacity
        )) {
        return DLINE_ERROR_CAPACITY_OVERFLOW;
    }

    DlineResult result = buffer_reserve_limited(
        buffer,
        required_capacity,
        maximum_capacity
    );

    if (result != DLINE_OK) {
        return result;
    }

    if (length != 0) {
        memcpy(buffer->data + buffer->length, data, length);
        buffer->length += length;
    }

    return DLINE_OK;
}

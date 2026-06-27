#ifndef DLINE_LIMIT_H
#define DLINE_LIMIT_H

#include <stdbool.h>
#include <stddef.h>

bool limit_checked_add(
    size_t left,
    size_t right,
    size_t maximum,
    size_t *result
);

bool limit_calculate_capacity(
    size_t current_capacity,
    size_t required_capacity,
    size_t initial_capacity,
    size_t maximum_capacity,
    size_t *result
);

#endif

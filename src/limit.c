#include "limit.h"

bool limit_checked_add(
    size_t left,
    size_t right,
    size_t maximum,
    size_t *result
) {
    if (result == NULL || left > maximum || right > maximum - left) {
        return false;
    }

    *result = left + right;
    return true;
}

bool limit_calculate_capacity(
    size_t current_capacity,
    size_t required_capacity,
    size_t initial_capacity,
    size_t maximum_capacity,
    size_t *result
) {
    if (result == NULL || required_capacity > maximum_capacity) {
        return false;
    }

    if (required_capacity <= current_capacity) {
        *result = current_capacity;
        return true;
    }

    size_t capacity = current_capacity;
    if (capacity == 0) {
        capacity = initial_capacity;
        if (capacity > maximum_capacity) {
            capacity = maximum_capacity;
        }
    }

    if (capacity == 0) {
        capacity = 1;
    }

    while (capacity < required_capacity) {
        size_t growth = capacity / 2;
        if (growth == 0) {
            growth = 1;
        }

        size_t next_capacity = 0;
        if (!limit_checked_add(
                capacity,
                growth,
                maximum_capacity,
                &next_capacity
            )) {
            capacity = maximum_capacity;
            break;
        }

        capacity = next_capacity;
    }

    if (capacity < required_capacity) {
        return false;
    }

    *result = capacity;
    return true;
}

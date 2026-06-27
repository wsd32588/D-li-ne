#include "save_load.h"

#include <stdio.h>

enum {
    SAVE_LOAD_READ_CHUNK_SIZE = 16 * 1024
};

static const size_t SAVE_LOAD_DEFAULT_MAXIMUM_BYTES =
    (size_t)256 * 1024 * 1024;

SaveLoadOptions save_load_default_options(void) {
    SaveLoadOptions options = {
        .maximum_file_bytes = SAVE_LOAD_DEFAULT_MAXIMUM_BYTES
    };

    return options;
}

DlineResult save_load_read_file(
    const char *path,
    const SaveLoadOptions *options,
    DlineBuffer *result
) {
    if (path == NULL || result == NULL) {
        return DLINE_ERROR_INVALID_ARGUMENT;
    }

    SaveLoadOptions effective_options = save_load_default_options();
    if (options != NULL) {
        effective_options = *options;
    }

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return DLINE_ERROR_IO;
    }

    DlineBuffer loaded = {0};
    DlineResult status = DLINE_OK;

    uint8_t chunk[SAVE_LOAD_READ_CHUNK_SIZE];

    for (;;) {
        size_t bytes_read = fread(chunk, 1, sizeof(chunk), file);

        if (bytes_read != 0) {
            status = buffer_append_limited(
                &loaded,
                chunk,
                bytes_read,
                effective_options.maximum_file_bytes
            );

            if (status == DLINE_ERROR_CAPACITY_OVERFLOW) {
                status = DLINE_ERROR_FILE_TOO_LARGE;
            }

            if (status != DLINE_OK) {
                break;
            }
        }

        if (bytes_read < sizeof(chunk)) {
            if (ferror(file)) {
                status = DLINE_ERROR_IO;
            }
            break;
        }
    }

    if (fclose(file) != 0 && status == DLINE_OK) {
        status = DLINE_ERROR_IO;
    }

    if (status != DLINE_OK) {
        buffer_destroy(&loaded);
        return status;
    }

    DlineBuffer previous = *result;
    *result = loaded;
    buffer_destroy(&previous);
    return DLINE_OK;
}

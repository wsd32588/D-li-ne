#include "buffer.h"
#include "limit.h"
#include "save_load.h"
#include "utf8.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_failure_count = 0;

static void expect_true(int condition, const char *message) {
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        g_failure_count++;
    }
}

static void test_utf8_decode_boundaries(void) {
    static const uint8_t maximum_codepoint[] = {
        0xF4, 0x8F, 0xBF, 0xBF
    };
    static const uint8_t above_maximum[] = {
        0xF4, 0x90, 0x80, 0x80
    };
    static const uint8_t overlong_sequence[] = {
        0xE0, 0x80, 0x80
    };
    static const uint8_t surrogate_sequence[] = {
        0xED, 0xA0, 0x80
    };

    Utf8Char decoded = utf8_decode(
        maximum_codepoint,
        sizeof(maximum_codepoint)
    );
    expect_true(
        decoded.validity == UTF8_VALID,
        "utf8_decode accepts U+10FFFF"
    );
    expect_true(
        decoded.codepoint == 0x10FFFF,
        "utf8_decode produces U+10FFFF"
    );

    decoded = utf8_decode(above_maximum, sizeof(above_maximum));
    expect_true(
        decoded.validity == UTF8_INVALID,
        "utf8_decode rejects codepoints above U+10FFFF"
    );

    decoded = utf8_decode(overlong_sequence, sizeof(overlong_sequence));
    expect_true(
        decoded.validity == UTF8_INVALID,
        "utf8_decode rejects overlong sequences"
    );

    decoded = utf8_decode(surrogate_sequence, sizeof(surrogate_sequence));
    expect_true(
        decoded.validity == UTF8_INVALID,
        "utf8_decode rejects surrogate codepoints"
    );
}

static void test_buffer_lifecycle(void) {
    static const uint8_t input[] = "Dline";
    DlineBuffer buffer = {0};

    DlineResult result = buffer_init(&buffer);
    expect_true(result == DLINE_OK, "buffer_init succeeds");
    if (result != DLINE_OK) {
        return;
    }

    expect_true(buffer.data == NULL, "buffer_init defers storage allocation");
    expect_true(buffer.length == 0, "buffer_init sets an empty length");
    expect_true(buffer.capacity == 0, "buffer_init starts with zero capacity");

    result = buffer_append(&buffer, input, sizeof(input) - 1);
    expect_true(result == DLINE_OK, "buffer_append succeeds");
    expect_true(buffer.data != NULL, "buffer_append allocates storage on demand");
    expect_true(
        buffer.length == sizeof(input) - 1,
        "buffer_append updates the length"
    );
    expect_true(
        memcmp(buffer.data, input, sizeof(input) - 1) == 0,
        "buffer_append copies input data"
    );

    result = buffer_reserve(&buffer, 1024);
    expect_true(result == DLINE_OK, "buffer_reserve grows the buffer");
    expect_true(buffer.capacity >= 1024, "buffer_reserve updates capacity");
    expect_true(
        memcmp(buffer.data, input, sizeof(input) - 1) == 0,
        "buffer_reserve preserves existing data"
    );

    result = buffer_append(&buffer, NULL, 0);
    expect_true(result == DLINE_OK, "buffer_append accepts empty input");

    result = buffer_append(&buffer, NULL, 1);
    expect_true(
        result == DLINE_ERROR_INVALID_ARGUMENT,
        "buffer_append rejects a null non-empty input"
    );

    buffer_destroy(&buffer);
    expect_true(buffer.data == NULL, "buffer_destroy clears the data pointer");
    expect_true(buffer.length == 0, "buffer_destroy clears the length");
    expect_true(buffer.capacity == 0, "buffer_destroy clears the capacity");
}

static void test_limit_calculations(void) {
    size_t result = 0;

    expect_true(
        limit_checked_add(10, 20, 30, &result) && result == 30,
        "limit_checked_add accepts a sum at the limit"
    );
    expect_true(
        !limit_checked_add(SIZE_MAX, 1, SIZE_MAX, &result),
        "limit_checked_add rejects size_t overflow"
    );
    expect_true(
        limit_calculate_capacity(0, 1000, 256, 1000, &result) &&
            result == 1000,
        "limit_calculate_capacity respects the maximum capacity"
    );
    expect_true(
        !limit_calculate_capacity(0, 1001, 256, 1000, &result),
        "limit_calculate_capacity rejects requirements above the limit"
    );
}

static void test_save_load_file(void) {
    static const char path[] = "dline_save_load_test.tmp";
    static const uint8_t contents[] = {
        'a', '\n', 'b', 0, 'c'
    };

    FILE *file = fopen(path, "wb");
    expect_true(file != NULL, "save_load test creates an input file");
    if (file == NULL) {
        return;
    }

    size_t bytes_written = fwrite(contents, 1, sizeof(contents), file);
    int close_result = fclose(file);
    expect_true(
        bytes_written == sizeof(contents) && close_result == 0,
        "save_load test writes the complete input file"
    );
    if (bytes_written != sizeof(contents) || close_result != 0) {
        return;
    }

    DlineBuffer buffer = {0};
    DlineResult result = buffer_init(&buffer);
    expect_true(result == DLINE_OK, "save_load result buffer initializes");

    SaveLoadOptions options = {
        .maximum_file_bytes = sizeof(contents)
    };
    result = save_load_read_file(path, &options, &buffer);
    expect_true(result == DLINE_OK, "save_load reads a file within its limit");
    expect_true(
        buffer.length == sizeof(contents),
        "save_load records the complete file length"
    );
    expect_true(
        memcmp(buffer.data, contents, sizeof(contents)) == 0,
        "save_load preserves binary file contents"
    );

    options.maximum_file_bytes = sizeof(contents) - 1;
    result = save_load_read_file(path, &options, &buffer);
    expect_true(
        result == DLINE_ERROR_FILE_TOO_LARGE,
        "save_load rejects a file above its configured limit"
    );
    expect_true(
        buffer.length == sizeof(contents) &&
            memcmp(buffer.data, contents, sizeof(contents)) == 0,
        "save_load preserves the previous buffer after failure"
    );

    buffer_destroy(&buffer);
}

static void test_public_error_description(void) {
    expect_true(
        strcmp(
            dline_result_string(DLINE_ERROR_OUT_OF_MEMORY),
            "out of memory"
        ) == 0,
        "dline_result_string describes out-of-memory errors"
    );
}

int main(void) {
    test_utf8_decode_boundaries();
    test_buffer_lifecycle();
    test_limit_calculations();
    test_save_load_file();
    test_public_error_description();

    if (g_failure_count != 0) {
        fprintf(stderr, "%d test assertion(s) failed\n", g_failure_count);
        return EXIT_FAILURE;
    }

    puts("All tests passed");
    return EXIT_SUCCESS;
}

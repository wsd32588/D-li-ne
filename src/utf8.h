#ifndef UTF8_H
#define UTF8_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    UTF8_INVALID = 0,
    UTF8_VALID
} Utf8Validity;

enum {
    UTF8_MAX_LENGTH = 4
};

typedef uint32_t UnicodeCodePoint;

typedef struct {
    uint8_t bytes[UTF8_MAX_LENGTH];
    uint8_t length;
} Utf8Sequence;

typedef struct {
    UnicodeCodePoint codepoint;
    uint8_t byte_length;
    Utf8Validity validity;
} Utf8Char;

Utf8Char utf8_decode(
    const uint8_t *bytes,
    size_t available
);

Utf8Validity utf8_encode(
    UnicodeCodePoint codepoint,
    Utf8Sequence *result
);

#endif

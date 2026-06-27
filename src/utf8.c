#include "utf8.h"

static const UnicodeCodePoint minimum_codepoints[] = {
    0,
    0,
    0x80,
    0x800,
    0x10000
};

static uint8_t utf8_get_length(uint8_t first_byte) {
    if (first_byte <= 0x7F) {
        return 1;
    }

    if (first_byte >= 0xC2 && first_byte <= 0xDF) {
        return 2;
    }

    if (first_byte >= 0xE0 && first_byte <= 0xEF) {
        return 3;
    }

    if (first_byte >= 0xF0 && first_byte <= 0xF4) {
        return 4;
    }

    return 0;
}
Utf8Char utf8_decode(
    const uint8_t *bytes,
    size_t available
) {
    Utf8Char result = {
        0xFFFD,
        0,
        UTF8_INVALID
    };
    if (available < 1 || bytes == NULL) return result;

    result.byte_length = 1;

    uint8_t length = utf8_get_length(bytes[0]);

    if (length > available || length < 1) return result;

    for (uint8_t i = 1; i < length; i++) {
        if ((bytes[i] & 0xc0) != 0x80) {
            return result;
        }
    }

    switch (length) {
        case 1:
            result.codepoint = bytes[0];
            break;
        case 2:
            result.codepoint = (
                (bytes[0] & 0x1F) << 6 |
                (bytes[1] & 0x3F));
            break;
        case 3:
            result.codepoint = (
                (bytes[0] & 0x0F) << 12 |
                (bytes[1] & 0x3F) << 6 |
                (bytes[2] & 0x3F));
            break;
        case 4:
            result.codepoint = (
                (bytes[0] & 0x07) << 18 |
                (bytes[1] & 0x3F) << 12 |
                (bytes[2] & 0x3F) << 6 |
                (bytes[3] & 0x3F));
                break;
        default:
            break;
    }

    if (result.codepoint < minimum_codepoints[length] ||
        result.codepoint > 0x10FFFF ||
        (result.codepoint >= 0xD800 && result.codepoint <= 0xDFFF)) {
        return result;
    }

    result.byte_length = length;
    result.validity = UTF8_VALID;
    return result;
}

Utf8Validity utf8_encode(
    UnicodeCodePoint codepoint,
    Utf8Sequence *result
) {
    if (result == NULL) return UTF8_INVALID;

    result->length = 0;

    if (codepoint > 0x10FFFF ||
        (codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
        return UTF8_INVALID;
    }

    if (codepoint <= 0x7F) {
        result->bytes[0] = (uint8_t) codepoint;
        result->length = 1;
    } else if (codepoint <= 0x7FF) {
        result->bytes[0] = (uint8_t) 0xC0 | (codepoint >> 6);
        result->bytes[1] = (uint8_t) 0x80 | (codepoint & 0x3F);
        result->length = 2;
    } else if (codepoint <= 0xFFFF) {
        result->bytes[0] = (uint8_t) 0xE0 | (codepoint >> 12);
        result->bytes[1] = (uint8_t) 0x80 | ((codepoint >> 6) & 0x3F);
        result->bytes[2] = (uint8_t) 0x80 | (codepoint & 0x3F);
        result->length = 3;
    } else {
        result->bytes[0] = (uint8_t) 0xF0 | (codepoint >> 18);
        result->bytes[1] = (uint8_t) 0x80 | ((codepoint >> 12) & 0x3F);
        result->bytes[2] = (uint8_t) 0x80 | ((codepoint >> 6) & 0x3F);
        result->bytes[3] = (uint8_t) 0x80 | (codepoint & 0x3F);
        result->length = 4;
    }
    return UTF8_VALID;
}

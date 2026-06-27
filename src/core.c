#include "Dline/core.h"

const char *dline_result_string(DlineResult result) {
    switch (result) {
        case DLINE_OK:
            return "success";
        case DLINE_ERROR_INVALID_ARGUMENT:
            return "invalid argument";
        case DLINE_ERROR_OUT_OF_MEMORY:
            return "out of memory";
        case DLINE_ERROR_CAPACITY_OVERFLOW:
            return "buffer capacity overflow";
        case DLINE_ERROR_INVALID_UTF8:
            return "invalid UTF-8";
        case DLINE_ERROR_IO:
            return "I/O error";
        case DLINE_ERROR_FILE_TOO_LARGE:
            return "file exceeds the configured size limit";
        default:
            return "unknown error";
    }
}

#ifndef DLINE_CORE_H
#define DLINE_CORE_H

#if defined(_WIN32) || defined(__CYGWIN__)
    #if defined(DLINE_CORE_EXPORTS)
        #define DLINE_API __declspec(dllexport)
    #else
        #define DLINE_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) && __GNUC__ >= 4
    #define DLINE_API __attribute__((visibility("default")))
#else
    #define DLINE_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DLINE_OK = 0,
    DLINE_ERROR_INVALID_ARGUMENT,
    DLINE_ERROR_OUT_OF_MEMORY,
    DLINE_ERROR_CAPACITY_OVERFLOW,
    DLINE_ERROR_INVALID_UTF8
} DlineResult;

DLINE_API const char *dline_result_string(DlineResult result);

#ifdef __cplusplus
}
#endif

#endif

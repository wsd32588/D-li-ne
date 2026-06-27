#include "Dline/core.h"

#include <stdlib.h>
#include <string.h>

int main(void) {
    const char *description = dline_result_string(
        DLINE_ERROR_OUT_OF_MEMORY
    );

    if (strcmp(description, "out of memory") != 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

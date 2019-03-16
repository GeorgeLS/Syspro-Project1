#ifndef EXERCISE_I_MACROS_H
#define EXERCISE_I_MACROS_H

#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
#include "IO_Utils.h"

#define __MALLOC(size, type) ((type*) malloc((size) * sizeof(type)))

#define MAX(x, y) ((x) < (y) ? (y) : (x))

#define ABS(n) \
({\
    int mask = n >> ((sizeof(int) * 8) - 1); \
    (mask + n) ^ mask; \
})

// A fast copy of a string with null termination
#define STR_COPY(dest, source, length) \
do { \
    memcpy(dest, source, length); \
    (dest)[length] = '\0'; \
} while (0) \

/*
 * This macro is used for copying a string in a safe manner.
 * We are using something called grouped bracket expression
 * which evaluates all statements in it and "returns" the
 * result of the last statement evaluated. So we are creating
 * a local string, allocating memory for it, check the result
 * of the memory allocation, copy it, and return it.
 */
#define SAFE_STR_COPY(str) \
({\
    size_t str_length = strlen(str); \
    char *str_copy = __MALLOC(str_length + 1, char); \
    if (str_copy == NULL) { \
        Report_Error("Couldn't allocate memory for copying string. Exiting..."); \
        exit(EXIT_FAILURE); \
    } \
    STR_COPY(str_copy, str, str_length); \
    str_copy; \
}) \

// A macro to do efficient string concatenation. Assumes that dest has the appropriate memory allocated
#define STR_CONCAT(dest, source, length) \
{\
    size_t dest_length = strlen(dest); \
    memcpy(dest + dest_length, source, length); \
    (dest)[dest_length + length] = '\0'; \
}\

#endif //EXERCISE_I_MACROS_H

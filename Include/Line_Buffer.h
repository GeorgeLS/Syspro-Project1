#ifndef EXERCISE_I_LINE_BUFFER_H
#define EXERCISE_I_LINE_BUFFER_H

#include <stddef.h>
#include <memory.h>
#include "Attributes.h"
#include "Macros.h"

typedef struct Line_Buffer {
    char *contents;
    size_t bytes;
} Line_Buffer;

static __INLINE__
Line_Buffer Line_Buffer_Create(void) {
    return (Line_Buffer) {
        .contents = NULL,
        .bytes = 0U
    };
}

static __INLINE__
Line_Buffer Line_Buffer_Create_With_Size(size_t size) {
    Line_Buffer line_buffer = {.bytes = size};
    line_buffer.contents = __MALLOC(size + 1, char);
    memset(line_buffer.contents, '\0', size + 1);
    return line_buffer;
}

static __INLINE__ __NON_NULL__(1)
void Line_Buffer_Delete(Line_Buffer *line_buffer) {
    free(line_buffer->contents);
    line_buffer->contents = NULL;
    line_buffer->bytes = 0U;
}

static __INLINE__ __NON_NULL__(1)
void Line_Buffer_Resize(Line_Buffer *line_buffer, size_t bytes) {
    line_buffer->contents = realloc(line_buffer->contents, bytes);
    line_buffer->bytes = bytes;
}

static __INLINE__ __NON_NULL__(1, 2)
void Line_Buffer_Write(Line_Buffer *line_buffer, const char *restrict data) {
    size_t data_length = strlen(data);
    if (line_buffer->bytes < data_length) {
        Line_Buffer_Resize(line_buffer, data_length);
    }
    memset(line_buffer->contents, '\0', line_buffer->bytes);
    STR_COPY(line_buffer->contents, data, data_length);
}

static __INLINE__ __NON_NULL__(1, 2)
void Line_Buffer_Append(Line_Buffer *line_buffer, const char *restrict data) {
    size_t data_length = strlen(data);
    size_t buffer_data_length = strlen(line_buffer->contents);
    if (line_buffer->bytes - buffer_data_length < data_length) {
        Line_Buffer_Resize(line_buffer, data_length + line_buffer->bytes + 1);
    }
    STR_CONCAT(line_buffer->contents + buffer_data_length, data, data_length);
}

#endif //EXERCISE_I_LINE_BUFFER_H

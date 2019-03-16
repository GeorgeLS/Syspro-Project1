#ifndef EXERCISE_I_FILE_READER_H
#define EXERCISE_I_FILE_READER_H

#include <stdint.h>
#include "Line_Buffer.h"

typedef struct File_Reader {
    int fd;
    Line_Buffer buffer;
    uint8_t eof : 1;
} File_Reader;

static __INLINE__
File_Reader File_Reader_Create(int fd) {
    return (File_Reader) {
            .buffer = Line_Buffer_Create_With_Size(200),
            .fd = fd
    };
}

static __INLINE__
void File_Reader_Delete(File_Reader *file_reader) {
    Line_Buffer_Delete(&file_reader->buffer);
}

void File_Reader_Read_Line(File_Reader *file_reader) __NON_NULL__(1);

#endif //EXERCISE_I_FILE_READER_H

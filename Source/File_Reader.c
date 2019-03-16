#include <ctype.h>
#include "../Include/File_Reader.h"

void File_Reader_Read_Line(File_Reader *file_reader) {
    int fd = file_reader->fd;
    ssize_t res;
    size_t bytes_read = 0U;
    char ch;
    memset(file_reader->buffer.contents, '\0', file_reader->buffer.bytes);
    while (1) {
        res = read(fd, &ch, sizeof(char));
        if (res == 0 || ch == '\n') break;
        ++bytes_read;
        if (bytes_read > file_reader->buffer.bytes) {
            Line_Buffer_Resize(&file_reader->buffer, file_reader->buffer.bytes << 1U);
        }
        file_reader->buffer.contents[bytes_read - 1] = ch;
    }
    if (bytes_read == 0) {
        file_reader->buffer.contents[0] = '\0';
        return;
    }
    char *contents = file_reader->buffer.contents;
    size_t i = 0U;
    size_t j = strlen(contents) - 1;
    if (res == 0) {
        ++j;
        file_reader->eof = 1;
    }
    // Trim excess whitespace
    while (isspace(contents[i])) ++i;
    while (isspace(contents[j])) --j;
    size_t offset = j - i + 1;
    memmove(contents, contents + i, offset);
    memset(contents + offset, '\0', file_reader->buffer.bytes - (offset + i));
}

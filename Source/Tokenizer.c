#include "../Include/Tokenizer.h"
#include <ctype.h>

int Tokenizer_Next_Token(Tokenizer *tokenizer, char **token_out) {
    char *stream = tokenizer->stream;
    if (!*stream) return 0;
    if (token_out != NULL) *token_out = stream;
    const char delimiter = tokenizer->delimiter;
    while (*stream != delimiter && *stream != '\0') ++stream;
    *stream = '\0';
    tokenizer->stream = ++stream;
    return 1;
}

size_t Tokenizer_Count_Remaining_Tokens(Tokenizer *tokenizer) {
    char *stream = tokenizer->stream;
    if (!*stream) return 0;
    size_t tokens = 0U;
    const char delimiter = tokenizer->delimiter;
    while (*stream) if (*stream++ == delimiter) ++tokens;
    return ++tokens;
}
#ifndef EXERCISE_I_TOKENIZER_H
#define EXERCISE_I_TOKENIZER_H

#include <stddef.h>
#include "Attributes.h"

typedef struct Tokenizer {
    char *stream;
    char delimiter;
} Tokenizer;

int Tokenizer_Next_Token(Tokenizer *tokenizer, char **token_out) __NON_NULL__(1);

size_t Tokenizer_Count_Remaining_Tokens(Tokenizer *tokenizer) __NON_NULL__(1);

#endif //EXERCISE_I_TOKENIZER_H

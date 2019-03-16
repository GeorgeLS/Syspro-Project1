#ifndef EXERCISE_I_COMMON_TYPES_H
#define EXERCISE_I_COMMON_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    char *balances_filename;
    char *transactions_filename;
    int64_t bitcoin_value;
    size_t sender_buckets;
    size_t receiver_buckets;
    size_t bucket_size;
} Program_Parameters;

extern Program_Parameters parameters;

typedef size_t (*Hash_Function)(const void *restrict data, const size_t bytes, const size_t buckets);
typedef void (*Delete_Function)(void *ptr);

#endif //EXERCISE_I_COMMON_TYPES_H

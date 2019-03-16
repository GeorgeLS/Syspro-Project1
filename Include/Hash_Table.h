#ifndef EXERCISE_I_HASH_TABLE_H
#define EXERCISE_I_HASH_TABLE_H

#include <stdint.h>
#include "Common_Types.h"
#include "Generic_List.h"

struct Wallet;

typedef struct Hash_Table {
    Generic_List *table;
    size_t n_buckets;
    size_t entries_per_bucket_node;
    Hash_Function hash_function;
} Hash_Table;

Hash_Table Hash_Table_Create(size_t n_buckets, size_t bucket_size, Hash_Function hash_function) __NON_NULL__(3);

void Hash_Table_Delete(Hash_Table *hash_table) __NON_NULL__(1);

uint8_t Hash_Table_Insert(Hash_Table *hash_table, struct Wallet *wallet) __NON_NULL__(1, 2);

struct Wallet *Hash_Table_Try_Get_Value(const Hash_Table *hash_table, const char *restrict walletID) __NON_NULL__(1, 2);

#endif //EXERCISE_I_HASH_TABLE_H

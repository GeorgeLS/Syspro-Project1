#include "../Include/Hash_Table.h"
#include "../Include/Macros.h"
#include "../Include/Wallet.h"

typedef struct Bucket_Node {
    Generic_List chain;
    Wallet **wallets;
    size_t free_index;
} Bucket_Node;

static Bucket_Node *create_new_bucket_node(size_t n_wallets) {
    Bucket_Node *bucket_node = __MALLOC(1, Bucket_Node);
    if (bucket_node == NULL) {
        Report_Error("Could not allocate memory for bucket node. Exiting...");
        exit(EXIT_FAILURE);
    }
    bucket_node->wallets = __MALLOC(n_wallets, Wallet*);
    if (bucket_node->wallets == NULL) {
        Report_Error("Could not allocate memory for wallets. Exiting...");
        exit(EXIT_FAILURE);
    }
    memset(bucket_node->wallets, '\0', n_wallets * sizeof(Wallet *));
    INIT_LIST_HEAD(&bucket_node->chain);
    bucket_node->free_index = 0U;
    return bucket_node;
}

static __INLINE__
void Bucket_Node_Delete(Bucket_Node *bucket_node) {
    for (size_t i = 0U; i != bucket_node->free_index; ++i) {
        Wallet_Delete(bucket_node->wallets[i]);
    }
    free(bucket_node->wallets);
    free(bucket_node);
}

static __INLINE__
uint8_t Bucket_Node_Is_Full(Bucket_Node *bucket_node, size_t entries_per_bucket_node) {
    return (uint8_t) (bucket_node->free_index == entries_per_bucket_node);
}

static __INLINE__
void Add_Wallet_To_Bucket_Node(Bucket_Node *bucket_node, Wallet *wallet) {
    bucket_node->wallets[bucket_node->free_index] = wallet;
    ++bucket_node->free_index;
}

Hash_Table Hash_Table_Create(size_t n_buckets, size_t bucket_size, Hash_Function hash_function) {
    size_t entries_per_bucket_node = bucket_size / sizeof(Wallet *);
    Hash_Table hash_table = {
            .n_buckets = n_buckets,
            .hash_function = hash_function,
            .entries_per_bucket_node = entries_per_bucket_node
    };
    Generic_List *table = __MALLOC(n_buckets, Generic_List);
    if (table == NULL) {
        Report_Error("Could not allocate memory for hash table array. Exiting...");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0U; i != n_buckets; ++i) {
        INIT_LIST_HEAD(&table[i]);
        Bucket_Node *bucket_node = create_new_bucket_node(entries_per_bucket_node);
        List_Add_Tail(&table[i], &bucket_node->chain);
    }
    hash_table.table = table;
    return hash_table;
}

void Hash_Table_Delete(Hash_Table *hash_table) {
    for (size_t i = 0U;  i != hash_table->n_buckets; ++i) {
        Generic_List *list = &hash_table->table[i];
        list_free(list, Bucket_Node, chain, Bucket_Node_Delete);
    }
    free(hash_table->table);
}

static Wallet *Hash_Table_Find_Value(const Hash_Table *hash_table, const char *restrict walletID) {
    size_t walletID_length = strlen(walletID);
    size_t bucket = hash_table->hash_function(walletID, walletID_length, hash_table->n_buckets);
    Generic_List *table = hash_table->table;
    if (List_Is_Empty(&table[bucket])) return NULL;
    size_t entries_per_bucket_node = hash_table->entries_per_bucket_node;
    Bucket_Node *bucket_node = NULL;
    list_for_each_entry(&table[bucket], bucket_node, chain) {
        Wallet **wallets = bucket_node->wallets;
        for (size_t i = 0U; i != entries_per_bucket_node; ++i) {
            if (wallets[i] != NULL && !strncmp(wallets[i]->walletID, walletID, walletID_length)) {
                return wallets[i];
            }
        }
    }
    return NULL;
}

uint8_t Hash_Table_Insert(Hash_Table *hash_table, Wallet *wallet) {
    char *walletID = wallet->walletID;
    if (Hash_Table_Find_Value(hash_table, walletID) != NULL) {
        Report_Warning("The WalletID <%s> already exists.", walletID);
        return 0U;
    }
    size_t walletID_length = strlen(walletID);
    size_t bucket = hash_table->hash_function(walletID, walletID_length, hash_table->n_buckets);
    Generic_List *table = hash_table->table;
    Bucket_Node *bucket_node = NULL;
    size_t entries_per_bucket_node = hash_table->entries_per_bucket_node;
    list_for_each_entry(&table[bucket], bucket_node, chain) {
        if (!Bucket_Node_Is_Full(bucket_node, entries_per_bucket_node)) {
            Add_Wallet_To_Bucket_Node(bucket_node, wallet);
            return 1U;
        }
    }
    Bucket_Node *new_bucket_node = create_new_bucket_node(entries_per_bucket_node);
    List_Add_Tail(&table[bucket], &new_bucket_node->chain);
    Add_Wallet_To_Bucket_Node(new_bucket_node, wallet);
    return 1U;
}

Wallet *Hash_Table_Try_Get_Value(const Hash_Table *hash_table, const char *restrict walletID) {
    return Hash_Table_Find_Value(hash_table, walletID);
}
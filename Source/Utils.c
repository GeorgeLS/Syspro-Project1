#include "../Include/Bitcoin.h"
#include "../Include/Macros.h"
#include "../Include/Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <syscall.h>

#define ID_SIZE 16

uint8_t String_To_I64(char *str, int64_t *value_out) {
    char *valid;
    *value_out = strtoll(str, &valid, 10);
    if (*valid != '\0') return 0;
    return 1;
}

char *Generate_ID(void) {
    char *id = __MALLOC(ID_SIZE + 1, char);
    // Come on guys update your kernel... All I want to do is getrandom(id, ID_SIZE, 0);
    syscall(SYS_getrandom, id, ID_SIZE, 0);
    for (size_t i = 0U; i != ID_SIZE; ++i) {
       id[i] = (char) (((uint16_t) id[i] % 79) + 48);
    }
    id[ID_SIZE] = '\0';
    return id;
}

char *Append_Transaction_ID_To_Transaction_String(const char *transaction_string) {
    char *transactionID = Generate_ID();
    size_t transID_length = strlen(transactionID);
    size_t trans_str_length = strlen(transaction_string);
    char *final_transaction_str = __MALLOC(transID_length + trans_str_length + 2, char);
    STR_COPY(final_transaction_str, transactionID, transID_length);
    STR_CONCAT(final_transaction_str, " ", 1);
    STR_CONCAT(final_transaction_str, transaction_string, trans_str_length);
    free(transactionID);
    return final_transaction_str;
}

uint8_t Is_Date(const char *str) {
    return (uint8_t) (strchr(str, '-') ? 1 : 0);
}

uint8_t Is_Time(const char *str) {
    return (uint8_t) (strchr(str, ':') ? 1 : 0);
}

size_t hash_function(const void *data, size_t bytes, size_t buckets) {
    size_t hash_value = 0U;
    size_t double_words = bytes / sizeof(uint64_t);
    for (size_t i = 0U; i != double_words; ++i, data += sizeof(uint64_t)) {
        hash_value += *(uint64_t *) data;
    }
    size_t remaining_bytes = bytes % sizeof(uint64_t);
    for (size_t i = 0U; i != remaining_bytes; ++i, data += sizeof(uint8_t)) {
        hash_value += *(uint8_t *) data;
    }
    hash_value %= buckets;
    return hash_value;
}

int AVL_Bitcoin_Cmp(AVL_Node *n1, void *n2) {
    Bitcoin *b1 = AVL_Get_Entry(n1, Bitcoin, avl_node);
    char *b2ID = n2;
    return strcmp(b1->bitcoinID, b2ID);
}

int AVL_Bitcoin_Node_Cmp(AVL_Node *n1, AVL_Node *n2) {
    Bitcoin *b1 = AVL_Get_Entry(n1, Bitcoin, avl_node);
    Bitcoin *b2 = AVL_Get_Entry(n2, Bitcoin, avl_node);
    return strcmp(b1->bitcoinID, b2->bitcoinID);
}
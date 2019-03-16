#ifndef EXERCISE_I_UTILS_H
#define EXERCISE_I_UTILS_H

#include <stdint.h>
#include <time.h>
#include "Attributes.h"
#include "AVL_Tree.h"

char *Generate_ID(void);

char *Append_Transaction_ID_To_Transaction_String(const char *transaction_string) __NON_NULL__(1);

uint8_t Is_Date(const char *str) __NON_NULL__(1);

uint8_t Is_Time(const char *str) __NON_NULL__(1);

size_t hash_function(const void *data, size_t bytes, size_t buckets) __NON_NULL__(1);

int AVL_Bitcoin_Cmp(AVL_Node *n1, void *data) __NON_NULL__(1, 2);

int AVL_Bitcoin_Node_Cmp(AVL_Node *n1, AVL_Node *n2) __NON_NULL__(1, 2);

uint8_t String_To_I64(char *str, int64_t *value_out) __NON_NULL__(1, 2);

#endif //EXERCISE_I_UTILS_H

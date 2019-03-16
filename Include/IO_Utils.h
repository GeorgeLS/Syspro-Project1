#ifndef EXERCISE_I_FILE_UTILS_H
#define EXERCISE_I_FILE_UTILS_H

#include "Attributes.h"
#include "AVL_Tree.h"
#include "Hash_Table.h"

struct Line_Buffer;

void Report_Error(const char *fmt, ...) __FORMAT__(printf, 1, 2);

void Report_Warning(const char *fmt, ...) __FORMAT__(printf, 1, 2);

void Report_Response(const char *fmt, ...) __FORMAT__(printf, 1, 2);

void Read_Line_From_Stdin(struct Line_Buffer *line_buffer);

void Parse_Transaction_And_Update_Structures(char *transaction_string, Hash_Table *hash_table, AVL_Tree *tree, uint8_t report_result)
__NON_NULL__(1, 2, 3);

void Parse_Multiple_Transactions_And_Update_Structures(char *transactions_string, Hash_Table *hash_table, AVL_Tree *tree)
__NON_NULL__(1, 2, 3);

void Read_Balances_File_And_Update_Structures(Hash_Table *hash_table, AVL_Tree *tree) __NON_NULL__(1, 2);

void Read_Transactions_File_And_Update_Structures(const char *filename, Hash_Table *hash_table, AVL_Tree *tree)
__NON_NULL__(1, 2, 3);

#endif //EXERCISE_I_FILE_UTILS_H

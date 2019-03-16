#ifndef EXERCISE_I_COMMANDS_H
#define EXERCISE_I_COMMANDS_H

#include "Attributes.h"
#include "Hash_Table.h"
#include "AVL_Tree.h"
#include "Bitcoin.h"

void
Command_Request_Transaction(char *transaction_string, Hash_Table *hash_table, AVL_Tree *tree) __NON_NULL__(1, 2, 3);

void
Command_Request_Multiple_Transactions_From_File(const char *restrict filename, Hash_Table *hash_table, AVL_Tree *tree)
__NON_NULL__(1, 2, 3);

void Command_Request_Multiple_Transactions(char *transactions_string, Hash_Table *hash_table, AVL_Tree *tree)
__NON_NULL__(1, 2, 3);

void Command_Find_Earnings(const char *walletID, char *interval_string, Hash_Table *hash_table) __NON_NULL__(1, 2, 3);

void Command_Find_Payments(const char *walletID, char *interval_string, Hash_Table *hash_table) __NON_NULL__(1, 2, 3);

void Command_Wallet_Status(const char *walletID, Hash_Table *hash_table) __NON_NULL__(1, 2);

void Command_Bitcoin_Status(Bitcoin *bitcoin) __NON_NULL__(1);

void Command_Trace_Coin(Bitcoin *bitcoin) __NON_NULL__(1);

#endif //EXERCISE_I_COMMANDS_H

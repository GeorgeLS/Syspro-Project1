#ifndef EXERCISE_I_BITCOIN_TRANSACTION_TREE_H
#define EXERCISE_I_BITCOIN_TRANSACTION_TREE_H

#include "Transaction.h"

// Bitcoin Transaction Tree Node
typedef struct Bitcoin_TTN {
    struct Bitcoin_TTN *left, *right;
    char *walletID;
    uint64_t amount;
    Transaction *transaction;
} Bitcoin_TTN;

// Bitcoin Transaction Tree
typedef struct Bitcoin_TT {
    Bitcoin_TTN *root;
} Bitcoin_TT;

Bitcoin_TT Bitcoin_TT_Create(const char *ownerID, uint64_t amount) __NON_NULL__(1);

void Bitcoin_TT_Delete(Bitcoin_TT *tree) __NON_NULL__(1);

void Bitcoin_TT_New_Transaction(Bitcoin_TT *tree, Transaction *transaction) __NON_NULL__(1, 2);

void Bitcoin_TT_Print_Transactions_Involved(Bitcoin_TT *tree) __NON_NULL__(1);

void Bitcoin_TT_Print_Status(struct Bitcoin *bitcoin) __NON_NULL__(1);


#endif //EXERCISE_I_BITCOIN_TRANSACTION_TREE_H
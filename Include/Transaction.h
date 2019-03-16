#ifndef EXERCISE_I_TRANSACTION_H
#define EXERCISE_I_TRANSACTION_H

#include <stdint.h>
#include "DateTime.h"
#include "Wallet.h"

typedef struct Transaction {
    char *transactionID;
    char *senderID;
    char *receiverID;
    uint64_t amount;
    DateTime dateTime;
} Transaction;

static __INLINE__ __NON_NULL__(1, 3)
Transaction *Transaction_Create(const char *restrict transactionID, const char *restrict senderID,
                                const char *restrict receiverID, uint64_t amount, DateTime *dateTime) {
    Transaction *transaction = __MALLOC(1, Transaction);
    transaction->transactionID = SAFE_STR_COPY(transactionID);
    transaction->senderID = SAFE_STR_COPY(senderID);
    transaction->receiverID = SAFE_STR_COPY(receiverID);
    transaction->dateTime = *dateTime;
    transaction->amount = amount;
    return transaction;
}

static __INLINE__ __NON_NULL__(1)
void Transaction_Delete(Transaction *transaction) {
    free(transaction->transactionID);
    free(transaction->senderID);
    free(transaction->receiverID);
    free(transaction);
}

void Do_Transaction(AVL_Tree *bitcoins, Wallet *sender_wallet, Wallet *receiver_wallet, Transaction *transaction)
__NON_NULL__(1, 2, 3, 4);

void Transcation_Print(Transaction *transaction) __NON_NULL__(1);


#endif //EXERCISE_I_TRANSACTION_H

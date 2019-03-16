#ifndef EXERCISE_I_WALLET_H
#define EXERCISE_I_WALLET_H

#include "Generic_List.h"
#include "Macros.h"

struct Bitcoin;

typedef struct Bitcoin_Wallet_Node {
    struct Bitcoin *bitcoin;
    uint64_t remaining_value;
    Generic_List node;
} Bitcoin_Wallet_Node;

typedef struct Wallet_Info {
    uint64_t total_balance;
    Generic_List bitcoin_list;
} Wallet_Info;

typedef struct Wallet {
    char *walletID;
    Wallet_Info info;
    Generic_List send_list;
    Generic_List received_list;
} Wallet;

Wallet *Wallet_Create(const char *walletID, uint64_t bitcoin_value, struct Bitcoin **bitcoins, size_t n_bitcoins)
__NON_NULL__(1, 3);

Wallet *Wallet_Create_Empty(const char *walletID) __NON_NULL__(1);

void Wallet_Delete(Wallet *wallet) __NON_NULL__(1);

void Wallet_Add_Bitcoin(Wallet *wallet, struct Bitcoin *bitcoin, uint64_t value) __NON_NULL__(1, 2);

void Wallet_Delete_Bitcoin_By_Node(Bitcoin_Wallet_Node *bwn) __NON_NULL__(1);

void Wallet_Print_Status(const Wallet *wallet) __NON_NULL__(1);

#endif //EXERCISE_I_WALLET_H

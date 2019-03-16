#include <stdio.h>
#include <inttypes.h>
#include "../Include/Wallet.h"
#include "../Include/Bitcoin.h"
#include "../Include/Pointer_Link.h"

static __INLINE__ __NON_NULL__(1)
Bitcoin_Wallet_Node *Bitcoin_Wallet_Node_Create(Bitcoin *bitcoin, uint64_t value) {
    Bitcoin_Wallet_Node *bwi = __MALLOC(1, Bitcoin_Wallet_Node);
    bwi->bitcoin = bitcoin;
    bwi->remaining_value = value;
    INIT_LIST_HEAD(&bwi->node);
    return bwi;
}

static __INLINE__ __NON_NULL__(1)
void Bitcoin_Wallet_Node_Delete(Bitcoin_Wallet_Node *v) {
//    Bitcoin_Delete(v->bitcoin);
    free(v);
}

Wallet *Wallet_Create(const char *walletID, uint64_t bitcoin_value, Bitcoin **bitcoins, size_t n_bitcoins) {
    Wallet *wallet = __MALLOC(1, Wallet);
    wallet->walletID = SAFE_STR_COPY(walletID);
    wallet->info.total_balance = bitcoin_value * n_bitcoins;
    INIT_LIST_HEAD(&wallet->info.bitcoin_list);
    for (size_t i = 0U; i != n_bitcoins; ++i) {
        Wallet_Add_Bitcoin(wallet, bitcoins[i], bitcoin_value);
    }
    INIT_LIST_HEAD(&wallet->send_list);
    INIT_LIST_HEAD(&wallet->received_list);
    return wallet;
}

Wallet *Wallet_Create_Empty(const char *walletID) {
    Wallet *wallet = __MALLOC(1, Wallet);
    wallet->walletID = SAFE_STR_COPY(walletID);
    wallet->info.total_balance = 0U;
    INIT_LIST_HEAD(&wallet->info.bitcoin_list);
    INIT_LIST_HEAD(&wallet->send_list);
    INIT_LIST_HEAD(&wallet->received_list);
    return wallet;
}

static __INLINE__
void Wallet_Delete_All_Transactions(Wallet *wallet) {
    Pointer_Link *link = NULL;
    list_for_each_entry(&wallet->send_list, link, node) {
        Transaction_Delete(link->ptr);
    }
    list_free(&wallet->send_list, Pointer_Link, node, free);
    list_free(&wallet->received_list, Pointer_Link, node, free);
}

void Wallet_Delete(Wallet *wallet) {
    list_free(&wallet->info.bitcoin_list, Bitcoin_Wallet_Node, node, Bitcoin_Wallet_Node_Delete);
    Wallet_Delete_All_Transactions(wallet);
    free(wallet->walletID);
    free(wallet);
}

void Wallet_Add_Bitcoin(Wallet *wallet, Bitcoin *bitcoin, uint64_t value) {
    Bitcoin_Wallet_Node *bwn = Bitcoin_Wallet_Node_Create(bitcoin, value);
    List_Add_Tail(&wallet->info.bitcoin_list, &bwn->node);
}

void Wallet_Delete_Bitcoin_By_Node(Bitcoin_Wallet_Node *bwn) {
    free(bwn);
}

void Wallet_Print_Status(const Wallet *wallet) {
    printf("Current Balance: %" PRIu64 "\n", wallet->info.total_balance);
}
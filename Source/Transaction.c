#include <stdio.h>
#include <inttypes.h>
#include "../Include/Transaction.h"
#include "../Include/Pointer_Link.h"
#include "../Include/Bitcoin_Transaction_Tree.h"
#include "../Include/Bitcoin.h"
#include "../Include/Utils.h"

void Do_Transaction(AVL_Tree *bitcoins, Wallet *sender_wallet, Wallet *receiver_wallet, Transaction *transaction) {
    Pointer_Link *transaction_link = Pointer_Link_Create(transaction);
    List_Add_Tail(&sender_wallet->send_list, &transaction_link->node);
    transaction_link = Pointer_Link_Create(transaction);
    List_Add_Tail(&receiver_wallet->received_list, &transaction_link->node);
    Wallet_Info *sender_winfo = &sender_wallet->info;
    Wallet_Info *receiver_winfo = &receiver_wallet->info;
    uint64_t amount = transaction->amount;
    sender_winfo->total_balance -= amount;
    receiver_winfo->total_balance += amount;
    Bitcoin_Wallet_Node *sbwn = NULL;
    Bitcoin_Wallet_Node *tmp = NULL;
    uint64_t aux_amount = amount;
    // The policy is that we choose to give the amount of the transaction from the first bitcoin available
    list_for_each_entry_safe(&sender_winfo->bitcoin_list, sbwn, tmp, node) {
        if (!aux_amount) break;
        uint8_t delete_sbwn = 0;
        uint64_t amount_given;
        // If the first bitcoin in the list has insufficient amount we give all the remaining amount from that bitcoin
        // and delete it. Continue this procedure until we give all the money. If the bitcoin has sufficient amount
        // then just give the money and stop.
        if (sbwn->remaining_value <= aux_amount) {
            amount_given = sbwn->remaining_value;
            aux_amount -= sbwn->remaining_value;
            delete_sbwn = 1;
        } else {
            amount_given = aux_amount;
            sbwn->remaining_value -= aux_amount;
            aux_amount = 0;
        }
        Bitcoin_Wallet_Node *rbwn = NULL;
        Bitcoin_Wallet_Node *iter = NULL;
        char *sbwn_bitcoinID = sbwn->bitcoin->bitcoinID;
        // Search the receiver's list of bitcoins to see if the bitcoin we are giving him already exists in his list.
        // If it does then just add amount to that bitcoin. Add the bitcoin to it's list otherwise
        list_for_each_entry(&receiver_winfo->bitcoin_list, iter, node) {
            char *iter_bitcoinID = iter->bitcoin->bitcoinID;
            if (!strcmp(iter_bitcoinID, sbwn_bitcoinID)) {
                rbwn = iter;
                break;
            }
        }
        // Search the bitcoin by it's ID in the set. From that bitcoin take it's transaction tree and update it.
        AVL_Node *bnode = AVL_Search(bitcoins, sbwn->bitcoin->bitcoinID);
        Bitcoin *bitcoin = AVL_Get_Entry(bnode, Bitcoin, avl_node);
        Bitcoin_TT_New_Transaction(&bitcoin->transaction_tree, transaction);
        if (!rbwn) Wallet_Add_Bitcoin(receiver_wallet, sbwn->bitcoin, amount_given);
        if (delete_sbwn) {
            List_Delete_Entry(&sbwn->node);
            Wallet_Delete_Bitcoin_By_Node(sbwn);
        }
    }
}

void Transcation_Print(Transaction *transaction) {
    printf("%s %s %s %" PRIu64 " %02d-%02d-%d %02d:%02d\n",
           transaction->transactionID, transaction->senderID,
           transaction->receiverID, transaction->amount,
           transaction->dateTime.day, transaction->dateTime.month,
           transaction->dateTime.year, transaction->dateTime.hour,
           transaction->dateTime.minute);
}
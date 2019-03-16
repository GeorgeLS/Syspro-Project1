#include <stdio.h>
#include <inttypes.h>
#include "../Include/Bitcoin.h"
#include "../Include/Bitcoin_Transaction_Tree.h"
#include "../Include/Pointer_Link.h"

static __INLINE__
Bitcoin_TTN *Bitcoin_TTN_Create(const char *walletID, uint64_t amount, Transaction *transaction) {
    Bitcoin_TTN *bitcoin_ttn = __MALLOC(1, Bitcoin_TTN);
    bitcoin_ttn->transaction = transaction;
    bitcoin_ttn->left = bitcoin_ttn->right = NULL;
    bitcoin_ttn->walletID = SAFE_STR_COPY(walletID);
    bitcoin_ttn->amount = amount;
    return bitcoin_ttn;
}

static __INLINE__
Bitcoin_TTN *Bitcoin_TTN_Create_Empty(void) {
    Bitcoin_TTN *bitcoin_ttn = __MALLOC(1, Bitcoin_TTN);
    bitcoin_ttn->transaction = NULL;
    bitcoin_ttn->left = bitcoin_ttn->right = NULL;
    return bitcoin_ttn;
}

static __INLINE__
void Bitcoin_TTN_Delete(Bitcoin_TTN *ttn) {
    free(ttn->walletID);
    free(ttn);
}

Bitcoin_TT Bitcoin_TT_Create(const char *ownerID, uint64_t amount) {
    return (Bitcoin_TT) {Bitcoin_TTN_Create(ownerID, amount, NULL)};
}

void Bitcoin_TT_Delete(Bitcoin_TT *tree) {
    LIST_HEAD(queue);
    Pointer_Link *link = Pointer_Link_Create(tree->root);
    List_Add_Tail(&queue, &link->node);
    while (!List_Is_Empty(&queue)) {
        Pointer_Link *_link = list_first_entry(&queue, Pointer_Link, node);
        Bitcoin_TTN *ttn = _link->ptr;
        List_Delete_Entry(&_link->node);
        free(_link);
        if (ttn->left) {
            link = Pointer_Link_Create(ttn->left);
            List_Add_Tail(&queue, &link->node);
        }
        if (ttn->right) {
            link = Pointer_Link_Create(ttn->right);
            List_Add_Tail(&queue, &link->node);
        }
        Bitcoin_TTN_Delete(ttn);
    }
}

static __NON_NULL__(1, 2, 3)
void Bitcoin_TT_Get_Leaf_Nodes_Of(Bitcoin_TT *tree, Generic_List *leaf_list, const char *restrict walletID) {
    LIST_HEAD(stack);
    Pointer_Link *link = Pointer_Link_Create(tree->root);
    List_Add(&stack, &link->node);
    while (!List_Is_Empty(&stack)) {
        Pointer_Link *_link = list_first_entry(&stack, Pointer_Link, node);
        Bitcoin_TTN *node = _link->ptr;
        List_Delete_Entry(&_link->node);
        free(_link);
        if (node->left) {
            link = Pointer_Link_Create(node->left);
            List_Add(&stack, &link->node);
        }
        if (node->right) {
            link = Pointer_Link_Create(node->right);
            List_Add(&stack, &link->node);
        }
        if (!node->left && !node->right) {
            if (!strcmp(node->walletID, walletID)) {
                link = Pointer_Link_Create(node);
                List_Add(leaf_list, &link->node);
            }
        }
    }
}

void Bitcoin_TT_New_Transaction(Bitcoin_TT *tree, Transaction *transaction) {
    LIST_HEAD(sender_leaf_nodes);
    Bitcoin_TT_Get_Leaf_Nodes_Of(tree, &sender_leaf_nodes, transaction->senderID);
    const char *senderID = transaction->senderID;
    const char *receiverID = transaction->receiverID;
    Pointer_Link *iter = NULL;
    uint64_t aux_amount = transaction->amount;
    uint8_t transaction_assigned = 0U;
    list_for_each_entry(&sender_leaf_nodes, iter, node) {
        if (!aux_amount) break;
        Bitcoin_TTN *node = iter->ptr;
        if (!transaction_assigned) {
            node->transaction = transaction;
            transaction_assigned = 1U;
        }
        uint64_t amount_given;
        uint64_t amount_remaining;
        if (node->amount > aux_amount) {
            amount_given = aux_amount;
            amount_remaining = node->amount - aux_amount;
            aux_amount = 0U;
        } else {
            amount_given = node->amount;
            amount_remaining = 0U;
            aux_amount -= node->amount;
        }
        Bitcoin_TTN *given = Bitcoin_TTN_Create(receiverID, amount_given, NULL);
        Bitcoin_TTN *remaining = NULL;
        if (amount_remaining != 0) {
            remaining = Bitcoin_TTN_Create(senderID, amount_remaining, NULL);
        }
        node->left = given;
        node->right = remaining;
    }
    list_free(&sender_leaf_nodes, Pointer_Link, node, free);
}

void Bitcoin_TT_Print_Transactions_Involved(Bitcoin_TT *tree) {
    LIST_HEAD(stack);
    Bitcoin_TTN *current = tree->root;
    while (current || !List_Is_Empty(&stack)) {
        while (current) {
            Pointer_Link *link = Pointer_Link_Create(current);
            List_Add(&stack, &link->node);
            current = current->left;
        }
        Pointer_Link *_link = list_first_entry(&stack, Pointer_Link, node);
        current = _link->ptr;
        if (current->transaction != NULL) {
            Transcation_Print(current->transaction);
        }
        List_Delete_Entry(&_link->node);
        free(_link);
        current = current->right;
    }
}

void Bitcoin_TT_Print_Status(Bitcoin *bitcoin) {
    Bitcoin_TT *tree = &bitcoin->transaction_tree;
    int64_t initial_amount = tree->root->amount;
    uint64_t n_transactions_involved = 0U;
    int64_t amount_used = 0;
    LIST_HEAD(stack);
    Bitcoin_TTN *current = tree->root;
    while (current || !List_Is_Empty(&stack)) {
        while (current) {
            Pointer_Link *link = Pointer_Link_Create(current);
            List_Add(&stack, &link->node);
            current = current->left;
        }
        Pointer_Link *_link = list_first_entry(&stack, Pointer_Link, node);
        current = _link->ptr;
        if (current->transaction != NULL) {
            ++n_transactions_involved;
            amount_used += current->transaction->amount;
        }
        List_Delete_Entry(&_link->node);
        free(_link);
        current = current->right;
    }
    printf("%s %" PRIu64" %" PRId64 "\n",
           bitcoin->bitcoinID, n_transactions_involved,
           (initial_amount - amount_used) < 0
           ? 0 : initial_amount - amount_used);
}
#ifndef EXERCISE_I_BITCOIN_H
#define EXERCISE_I_BITCOIN_H

#include "AVL_Tree.h"
#include "Macros.h"
#include "Bitcoin_Transaction_Tree.h"

typedef struct Bitcoin {
    char *bitcoinID;
    Bitcoin_TT transaction_tree;
    AVL_Node avl_node;
} Bitcoin;

static __INLINE__ __NON_NULL__(1)
Bitcoin *Bitcoin_Create(const char *restrict bitcoinID) {
    Bitcoin *bitcoin = __MALLOC(1, Bitcoin);
    bitcoin->bitcoinID = SAFE_STR_COPY(bitcoinID);
    return bitcoin;
}

static __INLINE__ __NON_NULL__(1)
void Bitcoin_Delete(Bitcoin *bitcoin) {
    free(bitcoin->bitcoinID);
    Bitcoin_TT_Delete(&bitcoin->transaction_tree);
    free(bitcoin);
}

#endif //EXERCISE_I_BITCOIN_H

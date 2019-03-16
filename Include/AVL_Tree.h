#ifndef EXERCISE_I_AVL_TREE_H
#define EXERCISE_I_AVL_TREE_H

#include <stddef.h>
#include "Attributes.h"
#include "Common_Types.h"

typedef struct AVL_Node {
    // We are using the 2 lowest bits of the parent for balance purposes.
    struct AVL_Node *parent, *left, *right;
} AVL_Node;

typedef int (*AVL_Node_Cmp_Function)(AVL_Node *n1, AVL_Node *n2);

typedef int (*AVL_Data_Cmp_Function)(AVL_Node *n1, void *n2);

typedef struct AVL_Tree {
    AVL_Node *root;
    AVL_Data_Cmp_Function data_cmp_function;
    AVL_Node_Cmp_Function node_cmp_function;
    Delete_Function delete_function;
} AVL_Tree;

#define AVL_Get_Entry(element, type, member) \
    ((type *) ((char*) (element) - offsetof(type, member)))

#define AVL_Parent(node) \
    ((AVL_Node *) ((uint64_t)(node)->parent & ~3))

#define AVL_Balance_Factor(node) \
    (((int)((uint64_t)(node)->parent & 3)) - 1)

void AVL_Tree_Init(AVL_Tree *tree, AVL_Data_Cmp_Function data_cmp_function,
                   AVL_Node_Cmp_Function node_cmp_function,
                   Delete_Function delete_function) __NON_NULL__(1);

void AVL_Tree_Delete(AVL_Tree *tree) __NON_NULL__(1);

AVL_Node *AVL_Insert(AVL_Tree *tree, AVL_Node *node) __NON_NULL__(1, 2);

AVL_Node *AVL_Search(AVL_Tree *tree, void *data) __NON_NULL__(1, 2);

#endif //EXERCISE_I_AVL_TREE_H

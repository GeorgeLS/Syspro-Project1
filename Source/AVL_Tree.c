#include <stddef.h>
#include <stdint.h>
#include "../Include/AVL_Tree.h"
#include "../Include/Macros.h"
#include "../Include/Pointer_Link.h"
#include "../Include/Bitcoin.h"

#define AVL_Get_Balance(node) ((node) ? (AVL_Balance_Factor(node)) : 0)

void AVL_Tree_Init(AVL_Tree *tree, AVL_Data_Cmp_Function data_cmp_function,
                   AVL_Node_Cmp_Function node_cmp_function,
                   Delete_Function delete_function) {
    tree->root = NULL;
    tree->data_cmp_function = data_cmp_function;
    tree->node_cmp_function = node_cmp_function;
    tree->delete_function = delete_function;
}

void AVL_Tree_Delete(AVL_Tree *tree) {
    LIST_HEAD(queue);
    Pointer_Link *link = Pointer_Link_Create(tree->root);
    List_Add_Tail(&queue, &link->node);
    while (!List_Is_Empty(&queue)) {
        Pointer_Link *_link = list_first_entry(&queue, Pointer_Link, node);
        AVL_Node *node = _link->ptr;
        List_Delete_Entry(&_link->node);
        free(_link);
        if (node->left) {
            link = Pointer_Link_Create(node->left);
            List_Add_Tail(&queue, &link->node);
        }
        if (node->right) {
            link = Pointer_Link_Create(node->right);
            List_Add_Tail(&queue, &link->node);
        }
        tree->delete_function(AVL_Get_Entry(node, Bitcoin, avl_node));
    }
}

static __INLINE__
void AVL_Set_Parent(AVL_Node *node, AVL_Node *parent) {
    node->parent = (AVL_Node *) ((uint64_t) parent | ((uint64_t) node->parent & 3));
}

static __INLINE__
void AVL_Set_Balance_Factor(AVL_Node *node, int factor) {
    node->parent = (AVL_Node *) ((uint64_t) AVL_Parent(node) | (uint64_t) (factor + 1));
}

static inline
AVL_Node *__Rotate_LL(AVL_Node *parent, int parent_factor, int *child_factor, int *h_delta) {
    AVL_Node *child = parent->left;
    int c_left = (child->left) ? 1 : 0;
    int c_right = (child->right) ? 1 : 0;
    int p_right;
    if (*child_factor < 0) {
        // child->left > child->right
        c_left = c_right - (*child_factor);
        p_right = c_left + parent_factor + 1;
        if (h_delta) {
            *h_delta = MAX(c_left, MAX(c_right, p_right) + 1) - (c_left + 1);
        }
    } else {
        // child->left <= child->right
        c_right = c_left + (*child_factor);
        p_right = c_right + parent_factor + 1;
        if (h_delta) {
            *h_delta = MAX(c_left, MAX(c_right, p_right) + 1) - (c_right + 1);
        }
    }
    *child_factor = (MAX(c_right, p_right) + 1) - c_left;
    AVL_Set_Balance_Factor(parent, p_right - c_right);
    parent->left = child->right;
    if (child->right) {
        AVL_Set_Parent(child->right, parent);
    }
    child->right = parent;
    AVL_Set_Parent(child, AVL_Parent(parent));
    AVL_Set_Parent(parent, child);
    return child;
}

static inline
AVL_Node *__Rotate_RR(AVL_Node *parent, int parent_factor, int *child_factor, int *h_delta) {
    AVL_Node *child = parent->right;
    int c_left = (child->left) ? 1 : 0;
    int c_right = (child->right) ? 1 : 0;
    int p_left;
    if (*child_factor < 0) {
        // child->left > child->right
        c_left = c_right - (*child_factor);
        p_left = c_left - parent_factor + 1;
        if (h_delta) {
            *h_delta = MAX(c_right, MAX(c_left, p_left) + 1) - (c_left + 1);
        }
    } else {
        // child->left <= child->right
        c_right = c_left + (*child_factor);
        p_left = c_right - parent_factor + 1;
        if (h_delta) {
            *h_delta = MAX(c_right, MAX(c_left, p_left) + 1) - (c_right + 1);
        }
    }
    *child_factor = c_right - (MAX(c_left, p_left) + 1);
    AVL_Set_Balance_Factor(parent, c_left - p_left);
    parent->right = child->left;
    if (child->left) {
        AVL_Set_Parent(child->left, parent);
    }
    child->left = parent;
    AVL_Set_Parent(child, AVL_Parent(parent));
    AVL_Set_Parent(parent, child);
    return child;
}

static inline
AVL_Node *__Rotate_LR(AVL_Node *parent, int parent_factor) {
    int h_delta = 0;
    AVL_Node *child = parent->left;
    int child_factor;
    if (child->right) {
        child_factor = AVL_Balance_Factor(child->right);
        parent->left = __Rotate_RR(child, AVL_Balance_Factor(child), &child_factor, &h_delta);
    } else {
        child_factor = AVL_Balance_Factor(child);
    }
    AVL_Node *res = __Rotate_LL(parent, parent_factor - h_delta, &child_factor, NULL);
    AVL_Set_Balance_Factor(res, child_factor);
    return res;
}

static inline
AVL_Node *__Rotate_RL(AVL_Node *parent, int parent_factor) {
    int h_delta = 0;
    struct AVL_Node *child = parent->right;
    int child_factor;
    if (child->left) {
        child_factor = AVL_Balance_Factor(child->left);
        parent->right = __Rotate_LL(child, AVL_Balance_Factor(child), &child_factor, &h_delta);
    } else {
        child_factor = AVL_Balance_Factor(child);
    }
    AVL_Node *res = __Rotate_RR(parent, parent_factor + h_delta, &child_factor, NULL);
    AVL_Set_Balance_Factor(res, child_factor);
    return res;
}

static AVL_Node *__Balance_Tree(AVL_Node *node, int balance_factor) {
    int h_delta = AVL_Get_Balance(node) + balance_factor;
    int child_factor;
    if (node) {
        if (h_delta < -1 && node->left) {
            // balance left subtree
            if (AVL_Get_Balance(node->left) <= 0) {
                child_factor = AVL_Balance_Factor(node->left);
                node = __Rotate_LL(node, h_delta, &child_factor, NULL);
                AVL_Set_Balance_Factor(node, child_factor);
            } else {
                node = __Rotate_LR(node, h_delta);
            }
        } else if (h_delta > 1 && node->right) {
            if (AVL_Get_Balance(node->right) >= 0) {
                child_factor = AVL_Balance_Factor(node->right);
                node = __Rotate_RR(node, h_delta, &child_factor, NULL);
                AVL_Set_Balance_Factor(node, child_factor);
            } else {
                node = __Rotate_RL(node, h_delta);
            }
        } else {
            AVL_Set_Balance_Factor(node, AVL_Balance_Factor(node) + balance_factor);
        }
    }
    return node;
}

AVL_Node *AVL_Insert(AVL_Tree *tree, AVL_Node *node) {
    AVL_Node *original = node;
    AVL_Node *parent = NULL;
    AVL_Node *current = tree->root;
    while (current) {
        int cmp_res = tree->node_cmp_function(current, node);
        parent = current;
        if (cmp_res > 0) {
            current = current->left;
        } else if (cmp_res < 0) {
            current = current->right;
        } else {
            return current;
        }
    }
    AVL_Set_Parent(node, parent);
    AVL_Set_Balance_Factor(node, 0);
    node->right = node->left = NULL;
    if (parent) {
        if (tree->node_cmp_function(parent, node) > 0) {
            parent->left = node;
        } else {
            parent->right = node;
        }
    } else {
        tree->root = node;
    }
    // Balancing process bottom up
    int balance_factor = 0;
    while (node) {
        parent = AVL_Parent(node);
        if (parent) {
            int old_balance_factor = AVL_Balance_Factor(node);
            if (parent->right == node) {
                node = __Balance_Tree(node, balance_factor);
                parent->right = node;
            } else {
                node = __Balance_Tree(node, balance_factor);
                parent->left = node;
            }
            // calculate balance factor for parent
            if (node->left == NULL && node->right == NULL) {
                if (parent->left == node) balance_factor = -1;
                else balance_factor = 1;
            } else {
                balance_factor = 0;
                if (ABS(old_balance_factor) < ABS(AVL_Balance_Factor(node))) {
                    if (parent->left == node) balance_factor = -1;
                    else balance_factor = 1;
                }
            }
        } else if (node == tree->root) {
            tree->root = __Balance_Tree(tree->root, balance_factor);
            break;
        }
        if (balance_factor == 0) break;
        node = parent;
    }
    return original;
}

AVL_Node *AVL_Search(AVL_Tree *tree, void *data) {
    AVL_Node *parent = tree->root;
    while (parent) {
        int cmp_res = tree->data_cmp_function(parent, data);
        if (cmp_res > 0) {
            parent = parent->left;
        } else if (cmp_res < 0) {
            parent = parent->right;
        } else {
            return parent;
        }
    }
    return NULL;
}
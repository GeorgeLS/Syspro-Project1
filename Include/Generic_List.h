#ifndef EXERCISE_I_GENERIC_LIST_H
#define EXERCISE_I_GENERIC_LIST_H

#include "Attributes.h"

typedef struct Generic_List {
    struct Generic_List *next, *prev;
} Generic_List;

#define container_of(ptr, type, member) ({ \
    const typeof(((type*) 0)->member) *__member_ptr = (ptr); \
    (type*) ((char*) __member_ptr - offsetof(type, member)); \
}) \

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

#define list_for_each(head, pos) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_entry(head, pos, member) \
    for (pos = list_entry((head)->next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_safe(head, pos, n, member)			\
	for (pos = list_first_entry(head, typeof(*pos), member),	\
		n = list_next_entry(pos, member);			\
	     &pos->member != (head); 					\
	     pos = n, n = list_next_entry(n, member))

#define list_free(head, type, member, deallocate_func) \
{ \
    type *tmp = NULL; \
    type *it = NULL; \
    list_for_each_entry_safe(head, it, tmp, member) { \
        List_Delete_Entry(&it->member); \
        deallocate_func(it); \
    } \
}

#define LIST_HEAD_INIT(struct_name) { &(struct_name), &(struct_name) }

#define LIST_HEAD(var_name) \
    Generic_List var_name = LIST_HEAD_INIT(var_name)

static __INLINE__
void INIT_LIST_HEAD(Generic_List *list) {
    list->next = list;
    list->prev = list;
}

static __INLINE__
void __list_add(Generic_List *new_node, Generic_List *previous_node, Generic_List *next_node) {
    next_node->prev = new_node;
    new_node->next = next_node;
    new_node->prev = previous_node;
    previous_node->next = new_node;
}

static __INLINE__
void __list_delete(Generic_List *previous, Generic_List *next) {
    next->prev = previous;
    previous->next = next;
}

static __INLINE__
uint8_t List_Is_Empty(const Generic_List *head) {
    return (uint8_t) (head->next == head);
}

static __INLINE__
void List_Add(Generic_List *head, Generic_List *new_node) {
    __list_add(new_node, head, head->next);
}

static __INLINE__
void List_Add_Tail(Generic_List *head, Generic_List *new_node) {
    __list_add(new_node, head->prev, head);
}

static __INLINE__
void List_Delete_Entry(Generic_List *entry) {
    __list_delete(entry->prev, entry->next);
}

#endif //EXERCISE_I_GENERIC_LIST_H

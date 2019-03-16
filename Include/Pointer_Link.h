#ifndef EXERCISE_I_POINTER_LINK_H
#define EXERCISE_I_POINTER_LINK_H

#include "Generic_List.h"
#include "Macros.h"

// Just a helper struct to represent a list of pointers to anything
typedef struct Pointer_Link {
    void *ptr;
    Generic_List node;
} Pointer_Link;

static __INLINE__ __NON_NULL__(1)
Pointer_Link *Pointer_Link_Create(void *ptr) {
    Pointer_Link *pointer_link = __MALLOC(1, Pointer_Link);
    pointer_link->ptr = ptr;
    INIT_LIST_HEAD(&pointer_link->node);
    return pointer_link;
}

#endif //EXERCISE_I_POINTER_LINK_H

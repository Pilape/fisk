#ifndef __LIST_HEADER__
#define __LIST_HEADER__

#include <stdlib.h>

/* == LIST ==
 * Copied straight from Tsoding
 */

/* == STRUCT REQUIREMENTS ==
 * element_type* items;
 * size_t count;
 * size_t capacity;
 *
 * size_t can be replaced with any integer.
 */

// Allocates the list with an initial capacity.
#define LIST_INIT(list, start_cap)                            \
do {                                                          \
    (list).capacity = start_cap;                              \
    (list).items = malloc(start_cap*sizeof((list).items[0])); \
} while (0)                                                   \


// Frees the memory of the list.
#define LIST_FREE(list) do { free((list).items); (list).items = NULL; } while(0)

// Appends the item to the end of the list and reallocates if needed.
#define LIST_APPEND(list, item)                                                         \
do {                                                                                    \
    if ((list).count >= (list).capacity) {                                              \
        if ((list).capacity == 0) (list).capacity = 1;                                  \
        (list).capacity *= 2;                                                           \
        (list).items = realloc((list).items, (list).capacity*sizeof((list).items[0]));  \
    }                                                                                   \
    (list).items[(list).count] = item;                                                  \
    (list).count++;                                                                     \
} while(0)                                                                              \

#endif

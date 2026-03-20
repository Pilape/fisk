#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef struct {
    int* items;
    size_t count;
    size_t capacity;
} int_list_t;

void ListPrint(int_list_t list) {
    for (int i=0; i<list.count; i++) {
        printf("[%d] = %d\n", i, list.items[i]);
    }
}

int main(int argc, char* argv[])
{
    int_list_t list = { 0 };
    LIST_INIT(list, 0);

    LIST_APPEND(list, 789);
    LIST_APPEND(list, 21);
    LIST_APPEND(list, 67);
    LIST_APPEND(list, 420);
    LIST_APPEND(list, 69);
    LIST_APPEND(list, 123);
    LIST_APPEND(list, 321);
    ListPrint(list);

    LIST_FREE(list);

    return 0;
}

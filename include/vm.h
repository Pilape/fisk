#ifndef __VM_HEADER__
#define __VM_HEADER__

#include "config.h"

typedef struct {
    enum {
        TYPE_NUMBER,
        TYPE_BOOL,
        TYPE_STRING,
        TYPE_LIST,
    } type;

    union {
        int number;
        char boolean;
    } value;
} langvalue_t;

typedef struct {
    unsigned int ip;
    unsigned int line; // For debug purposes
    char bytecode[MAX_TOKEN_COUNT];

    struct {
        langvalue_t data[STACK_SIZE];
        unsigned int ptr;
    } datastack;

    struct {
        unsigned int data[STACK_SIZE];
        unsigned int ptr;
    } returnstack;
} langvm_t;

#endif

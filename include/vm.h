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
    char bytecode[BYTECODE_MEMORY_SIZE];

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

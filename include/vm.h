#ifndef __VM_HEADER__
#define __VM_HEADER__

#include "config.h"

typedef enum {
    VM_ADD,
    VM_SUB,
    VM_MUL,
    VM_DIV,
    VM_MOD,
    VM_NUM,
    VM_LINE,
    VM_TRUE,
    VM_FALSE,
    VM_LIST_START,
    VM_LIST_END,
    VM_IF,
    VM_IFELSE,
    VM_AND,
    VM_OR,
    VM_NOT,
} langvm_instruction_t;

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

#define BYTECODE_ARRLEN 0xFFFF

typedef struct {
    unsigned int ip;
    unsigned int line; // For debug purposes
    char bytecode[BYTECODE_ARRLEN];

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

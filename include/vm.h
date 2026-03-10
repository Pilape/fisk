#ifndef __VM_HEADER__
#define __VM_HEADER__

#include <stdint.h>
#include <stdio.h>
#include "config.h"

typedef enum {
    VM_PUSH_INT,
    VM_PUSH_TRUE,
    VM_PUSH_FALSE,

    VM_START_LIST,
    VM_END_LIST,

} langbytecode_t;

typedef struct {
    size_t size, length;
} langlist_t;

typedef struct {
    enum {
        INT,
        BOOL,
        LIST,
    } type;
    union {
        int16_t integer;
        uint8_t boolean;
    } value;

} langcellvalue_t;

typedef struct {
    langcellvalue_t data[STACK_SIZE];
    int16_t ptr;
} langstack_t;

typedef struct {
    uint8_t bytecode[0x10000];
    uint16_t ip;

    langstack_t data_stack;
    struct {
        uint16_t data[STACK_SIZE];
        int16_t ptr;
    } return_stack;

} langvm_t;

#endif

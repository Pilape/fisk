#ifndef __LANG_HEADER__
#define __LANG_HEADER__

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/* ========================= STRUCTS ======================== */
/*
 * Simple utility used for reporting errors.
 */
typedef struct {
    int temp;
} lang_errhandler_t;

/* 
 * The virtual machine/runtime.
 * Does the stuff.
 */
typedef struct {
    uint16_t ip;
    uint8_t bytecode[0x10000];
} lang_vm_t;

/*
 * A linked list of function definitions and their address in bytecode.
 */
typedef struct __langfunction__ {
    char name[32];
    uint16_t address;

    struct __langfunction__* next; 
} lang_function_t;

/*
 * A lang process.
 */
typedef struct {
    lang_errhandler_t error_handler;
    lang_vm_t vm;

    uint16_t code_size;
    lang_function_t* functions;
} lang_state_t;

/* ========================= ERROR_HANDLING ======================== */
#include "src/lang_error.c"

/* ========================= SCANNER ======================== */
typedef enum {

    /* Literals */
    TOKEN_INT, 
    TOKEN_TRUE, TOKEN_FALSE,
        
    /* Single char */
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH, TOKEN_PERCENT, TOKEN_EQUAL,

    TOKEN_CURLY_L, TOKEN_CURLY_R, TOKEN_SQUARE_L, TOKEN_SQUARE_R,

    /* Keywords */
    /* Stack manipulation */
    TOKEN_SWAP, TOKEN_DROP, TOKEN_DUP,

    /* Logic */
    TOKEN_NOT, TOKEN_AND, TOKEN_OR,

    /* Control flow */
    TOKEN_IF, TOKEN_IFELSE, TOKEN_FOR, TOKEN_WHILE,

    /* Default */
    TOKEN_IDENTIFIER

} lang_tokentype_t;

typedef struct {
    lang_tokentype_t type;
    size_t line, length;
    char* start;
} lang_token_t;

typedef struct {
    size_t capacity;
    size_t length;
    lang_token_t* tokens;
} lang_tokenlist_t;

lang_tokenlist_t Scan(char* input, size_t length);

#include "src/lang_scanner.c"


/* ========================= CODEGEN ======================== */
#include "src/lang_codegen.c"

/* =========================== VM ========================== */

/*
 * The values the language supports.
 * Used in the datastack and variables.
 */
typedef struct {
    enum {
        LANG_TYPE_INT,
        LANG_TYPE_BOOL,
        LANG_TYPE_QUOTATION,
        LANG_TYPE_STRING
    } type;
} lang_value_t;

#include "src/lang_vm.c"

#endif

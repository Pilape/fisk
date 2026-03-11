#ifndef __LANG_HEADER__
#define __LANG_HEADER__

#include <stdlib.h>
#include <stdint.h>

/*
 * Simple utility used for reporting errors.
 */
typedef struct {

} langerrhandler_t;


/*
 * The values the language supports.
 * Used in the datastack and variables.
 */
typedef struct {
    enum {
        LANGTYPE_INT,
        LANGTYPE_BOOL,
        LANGTYPE_LIST,
        LANGTYPE_STRING,
    } type;
} langvalue_t;

/* 
 * The virtual machine/runtime.
 * Does the stuff.
 */
typedef struct {
    uint16_t ip;
    uint8_t bytecode[0x10000];
} langvm_t;

/*
 * A linked list of function definitions and their address in bytecode.
 */
typedef struct __langfunction__ {
    char name[32];
    uint16_t address;

    struct __langfunction__* next; 
} langfunction_t;

/*
 * A lang process.
 */
typedef struct {
    langvm_t vm;

    langfunction_t* functions;
    uint16_t code_size;
} langstate_t;

#include "src/lang_scanner.c"
#include "src/lang_error.c"
#include "src/lang_codegen.c"
#include "src/lang_vm.c"

#endif

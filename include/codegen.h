#ifndef __CODEGEN_HEADER__
#define __CODEGEN_HEADER__

#include "scanner.h"
#include "vm.h"
#include "config.h"
#include <stdint.h>

void GenerateBytecode(tokenlist_t token_list, langvm_t* vm);

typedef struct __langfunc__ {
    uint8_t is_foreign;
    union {
        uint16_t address;
        void (*func)(langstack_t* stack);
    } data;

    char name[FUNCNAME_SIZE];

    struct __langfunc__* next;
} langfunc_t;

#endif

#ifndef __LANG_HEADER__
#define __LANG_HEADER__

#include "include/scanner.h"
#include "include/codegen.h"
#include "include/vm.h"
#include <stdint.h>

typedef struct {
    langvm_t vm;

    langfunc_t* functions;
    uint16_t code_length;
} lang_t;

#endif

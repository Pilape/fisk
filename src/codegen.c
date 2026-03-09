#include "../lang.h"
#include "../include/codegen.h"
#include "../include/vm.h"
#include "../include/scanner.h"
#include <stdlib.h>
#include <stdio.h>

static void EmitByte(char byte, languint_t* program_size, char* bytecode) {
    if (*program_size >= BYTECODE_ARRLEN) {
        printf("[ERROR]: Bytecode overflow\n");
        exit(-1);
    }


}

static void EmitAddress();

void GenerateBytecode(tokenlist_t token_list, langctx_t* context) {
    languint_t program_size = 0;
    for (int i=0; i<token_list.token_count; i++) {
        switch (token_list.tokens[i].type) {
            case TOKEN_PLUS:
                break;

            default:
                // TODO: error
                break;

        }
    }
}

#undef EmitBytecode

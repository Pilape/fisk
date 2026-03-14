#include "../lang.h"

void Lang_GenerateBytecode(lang_tokenlist_t token_list, lang_vm_t* vm) {
    {int i; for (i=0; i<token_list.length; i++) {
        switch (token_list.tokens[i].type) {
            case TOKEN_IDENTIFIER:
                break;

            case TOKEN_INT:
                break;
            case TOKEN_TRUE:
                break;
            case TOKEN_FALSE:
                break;

            case TOKEN_SWAP:
                break;
            case TOKEN_DROP:
                break;
            case TOKEN_DUP:
                break;

            case TOKEN_PLUS:
                break;
            case TOKEN_MINUS:
                break;
            case TOKEN_STAR:
                break;
            case TOKEN_SLASH:
                break;
            case TOKEN_PERCENT:
                break;

            case TOKEN_EQUAL:
                break;
            case TOKEN_OR:
                break;
            case TOKEN_AND:
                break;
            case TOKEN_NOT:
                break;

            case TOKEN_IF:
                break;
            case TOKEN_IFELSE:
                break;
            case TOKEN_FOR:
                break;
            case TOKEN_WHILE:
                break;

            case TOKEN_CURLY_L:
                break;
            case TOKEN_CURLY_R:
                break;

            case TOKEN_SQUARE_L:
                break;
            case TOKEN_SQUARE_R:
                break;


        }
    }}
}

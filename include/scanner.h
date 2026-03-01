#ifndef __SCANNER_HEADER__
#define __SCANNER_HEADER__

#include "../include/config.h"
#include <stdlib.h>

typedef struct {
    enum {

        // Literals
        NUMBER, STRING,
        
        // Single char
        PLUS, MINUS, STAR, SLASH, PERCENT, EQUAL,
        CURLY_L, CURLY_R, SQUARE_L, SQUARE_R,

        // Keywords
        SWAP, DROP, DUP,

        // Default
        FUNCTION,


    } type;
    char lexeme[TOKEN_SIZE_LIMIT+1];
} token_t;

typedef struct {
    size_t capacity;
    size_t length;
    token_t* tokens;
} tokenlist_t;

tokenlist_t Scan(char* input, size_t length);

#endif

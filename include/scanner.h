#ifndef __SCANNER_HEADER__
#define __SCANNER_HEADER__

#include "../include/config.h"
#include <stdlib.h>

typedef enum {

    // Literals
    TOKEN_NUMBER, TOKEN_STRING,
        
    // Single char
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH, TOKEN_PERCENT, TOKEN_EQUAL,
    TOKEN_CURLY_L, TOKEN_CURLY_R, TOKEN_SQUARE_L, TOKEN_SQUARE_R,

    // Keywords
    TOKEN_SWAP, TOKEN_DROP, TOKEN_DUP,
    TOKEN_NOT, TOKEN_AND, TOKEN_OR,
    TOKEN_IF, TOKEN_IFELSE, TOKEN_FOR, TOKEN_WHILE,

    TOKEN_IDENTIFIER,

} tokentype_t;

typedef struct {
    tokentype_t type;
    unsigned int line, length;
    char* start;
} token_t;

typedef struct {
    size_t capacity;
    size_t length;
    token_t* tokens;
} tokenlist_t;

tokenlist_t Scan(char* input, size_t length);

#endif

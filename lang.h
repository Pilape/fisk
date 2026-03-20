#ifndef __LANG_HEADER__
#define __LANG_HEADER__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "list.h"

void Lang_Eval(char* str);

#endif

#ifdef __LANG_IMPLEMENTATION__
#undef __LANG_IMPLEMENTATION__

/* =============== SCANNER =============== */
typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_INT,
    TOKEN_STRING,
    TOKEN_QUOTATION,
    TOKEN_FUNCTION,
} _lang_tokentype_t;

typedef struct {
    _lang_tokentype_t type;
    size_t line;

    size_t lex_start;
    size_t lex_length;
} _lang_token_t;

typedef struct {
    _lang_token_t* items;
    size_t count;
    size_t capacity;
} _lang_tokenlist_t;

_lang_tokenlist_t _Lang_Scan(char* str, size_t str_length) {
    _lang_tokenlist_t tokens = { 0 };
    LIST_INIT(tokens, 32);

    size_t line = 1;
    size_t start = 0;
    size_t current = 0;

    while (current < str_length) {

        switch (str[current]) {
            case '\n':
                line++;
            case ' ':
            case '\t':
                break;

            case '(':
                while (current++ < str_length) {
                    if (str[current] == '\n') line++;
                    if (str[current] == ')') break;
                }
                break;

            default: {
                while (current++ < str_length) {
                    if (str[current] == '\n') { line++; break; }
                    if (str[current] == ' ' || str[current] == '\t') break;
                }
                _lang_token_t token = {
                    .type = 0,
                    .lex_start = start,
                    .lex_length = current-start,
                    .line = line,
                };
                LIST_APPEND(tokens, token);
                break;
            }
                
        }


        current++;
        start = current;
    }

    return tokens;
}
/* =============== CODEGEN =============== */



/* =============== RUNTIME =============== */

/* =============== GENERAL =============== */
void Lang_Eval(char* str) {
    _lang_tokenlist_t tokens = _Lang_Scan(str, strlen(str));

    // Print token list
    for (int i=0; i<tokens.count; i++) {
        _lang_token_t token = tokens.items[i];
        // General info
        printf("[%d]: { line=%zu, type=%d, '", i, token.line, token.type);
        // Lexeme
        for (int j=token.lex_start; j<token.lex_start+token.lex_length; j++) {
            printf("%c", str[j]);
        }
        // Close the brackets
        printf("' }\n");
    }

    LIST_FREE(tokens);
}

#endif

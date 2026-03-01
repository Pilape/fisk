#include "../include/scanner.h"
#include <stdio.h>
#include <string.h>

static inline tokenlist_t TokenListInit(unsigned int start_cap) {
    tokenlist_t list = { 0 };
    list.capacity = start_cap;

    list.tokens = malloc(sizeof(token_t) * start_cap);
    if (list.tokens == NULL) {
        printf("Oopsies, tokenlist couldn't allocate memory :(\n");
        exit(-1);
    }
    return list;
}

static inline void TokenListAppend(tokenlist_t* list, token_t token) {
    list->tokens[list->length] = token;

    list->length++;
    if (list->length >= list->capacity) {
        list->capacity *= 2;
        token_t* temp = realloc(list->tokens, sizeof(token_t)*list->capacity);

        if (temp == NULL) {
            printf("My bad gang. tokenlist ran out of memory to use :(\n");
            exit(-1);
        }
        list->tokens = temp;
    }
}

void EmitToken(char* code, size_t start, size_t end, unsigned int line, tokentype_t type, tokenlist_t* token_list) {
    token_t new_token = {
        .line = line,
        .length = end-start,
        
        .start = code + start,
        .type = type,
    };
    TokenListAppend(token_list, new_token);
}

tokenlist_t Scan(char* input, size_t length) {
    tokenlist_t token_list = TokenListInit(16);

    size_t i = 0;
    unsigned int line = 1;
    while (i<length) {
        switch (input[i]) {
            case '\n':
                line++;
            case ' ':
            case '\t':
                i++;
                break;

            case '(': {
                while (i++ < length) {
                    if (input[i] == '\n') line++;
                    if (input[i] == ')') break;
                }
                i++; // Consume the ')' character
                break;
            } 

            case '"': {
                size_t start = i;
                while (i++ < length) {
                    if (input[i] == '\n') line++;
                    if (input[i] == '"') break;
                }
                i++;
                EmitToken(input, start, i, line, TOKEN_STRING, &token_list);
                break;
            }

            default: {
                size_t start = i;
                while (i++ < length) {  
                    if (input[i] == ' ' || input[i] == '\n' || input[i] == '\t' || input[i] == '(') break;
                }
                EmitToken(input, start, i, line, 0, &token_list);
                
                break;
            }
                
        }
    }
    return token_list;
}



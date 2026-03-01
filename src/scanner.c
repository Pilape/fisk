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
    strcpy(list->tokens[list->length].lexeme, token.lexeme);

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

tokenlist_t Scan(char* input, size_t length) {
    tokenlist_t token_list = TokenListInit(16);

    size_t i = 0;
    while (i<length) {
        switch (input[i]) {
            case ' ':
            case '\n':
            case '\t':
                i++;
                break;

            default: {
                size_t start = i;
                while (i++ < length) { 
                    if (input[i] == ' ' || input[i] == '\n' || input[i] == '\t') break; 
                }

                token_t new_token = { 0 };
                unsigned int lexeme_length = i-start;
                if (lexeme_length >= TOKEN_SIZE_LIMIT) {
                    printf("[ERROR]: Token is too large\n");
                    // TODO better error handling (no panic exiting)
                    exit(-1);
                }

                for (int j=0; j<lexeme_length; j++) {
                    new_token.lexeme[j] = input[start+j];
                }
                new_token.lexeme[lexeme_length] = '\0';
                TokenListAppend(&token_list, new_token);

                break;
            }
                
        }
    }
    return token_list;
}



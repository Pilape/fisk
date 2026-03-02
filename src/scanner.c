#include "../include/scanner.h"
#include <stdio.h>

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

static void EmitToken(char* code, size_t start, size_t end, unsigned int line, tokentype_t type, tokenlist_t* token_list) {
    token_t new_token = {
        .line = line,
        .length = end-start,
        
        .start = code + start,
        .type = type,
    };
    TokenListAppend(token_list, new_token);
}

static int LexemeEquals(char* source, size_t start, size_t end, char* str, size_t str_length) {
    int length = end-start;
    if (length != str_length) return 0;

    for (int i=0; i<length; i++) {
        if (source[start+i] != str[i]) return 0;
    }

    return 1;
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

                tokentype_t type = TOKEN_IDENTIFIER;

                // This is probably "fine" for perfomance. Atleast for now
                // Kinda messed up tho

                // Fucked up macro
                // Got too verbose my bad gang
                // We're doing this to avoid using strlen() which iterates through the string. Instead we get it's length at compile time.
                #define IsKeyword(str) LexemeEquals(input, start, i, str, sizeof(str))
                
                // Stack manipulation
                if (IsKeyword("swap")) type = TOKEN_SWAP;
                else if (IsKeyword("dup")) type = TOKEN_DUP;
                else if (IsKeyword("drop")) type = TOKEN_DROP;

                // Math
                else if (IsKeyword("+")) type = TOKEN_PLUS;
                else if (IsKeyword("-")) type = TOKEN_MINUS;
                else if (IsKeyword("*")) type = TOKEN_STAR;
                else if (IsKeyword("/")) type = TOKEN_SLASH;
                else if (IsKeyword("%")) type = TOKEN_PERCENT;

                // Logic
                else if (IsKeyword("=")) type = TOKEN_EQUAL;
                else if (IsKeyword("or")) type = TOKEN_OR;
                else if (IsKeyword("and")) type = TOKEN_AND;
                else if (IsKeyword("not")) type = TOKEN_NOT;

                // Control flow
                else if (IsKeyword("if")) type = TOKEN_IF;
                else if (IsKeyword("ifelse")) type = TOKEN_IFELSE;
                else if (IsKeyword("for")) type = TOKEN_FOR;
                else if (IsKeyword("while")) type = TOKEN_WHILE;

                #undef IsKeyword

                EmitToken(input, start, i, line, type, &token_list); 
                break;
            }
                
        }
    }
    return token_list;
}

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

static inline int IsDigit(char c) {
    if (c >= '0' && '9' >= c) return 1;
    return 0;
}

static int IsNumber(char* source, size_t start, size_t end) {
    for (int i=start; i<end; i++) {
        if (!IsDigit(source[i])) return 0;
    }
    return 1;
}

static int LexemeEquals(char* source, size_t start, size_t end, char* str, size_t str_length) {
    int length = end-start;
    if (length != str_length) return 0;

    for (int i=0; i<length; i++) {
        if (source[start+i] != str[i]) return 0;
    }

    return 1;
}

// This is probably "fine" for perfomance. Atleast for now
// Kinda messed up tho
static inline tokentype_t TokenAssignType(char* source, size_t start, size_t end) {
    // Fucked up macro
    // Got too verbose my bad gang
    // We're doing this to avoid using strlen() which iterates through the string. Instead we get it's length at compile time.
    // We also have to subtract the null terminator from the length
    #define IsKeyword(str) LexemeEquals(source, start, end, str, sizeof(str)-1)
    
    // Number
    if (IsNumber(source, start, end)) return TOKEN_NUMBER;

    // Stack manipulation
    else if (IsKeyword("swap")) return TOKEN_SWAP;
    else if (IsKeyword("dup")) return TOKEN_DUP;
    else if (IsKeyword("drop")) return TOKEN_DROP;

    // Math
    else if (IsKeyword("+")) return TOKEN_PLUS;
    else if (IsKeyword("-")) return TOKEN_MINUS;
    else if (IsKeyword("*")) return TOKEN_STAR;
    else if (IsKeyword("/")) return TOKEN_SLASH;
    else if (IsKeyword("%")) return TOKEN_PERCENT;

    // Logic
    else if (IsKeyword("=")) return TOKEN_EQUAL;
    else if (IsKeyword("or")) return TOKEN_OR;
    else if (IsKeyword("and")) return TOKEN_AND;
    else if (IsKeyword("not")) return TOKEN_NOT;

    // Control flow
    else if (IsKeyword("if")) return TOKEN_IF;
    else if (IsKeyword("ifelse")) return TOKEN_IFELSE;
    else if (IsKeyword("for")) return TOKEN_FOR;
    else if (IsKeyword("while")) return TOKEN_WHILE;

    // Booleans
    else if (IsKeyword("true")) return TOKEN_TRUE;
    else if (IsKeyword("false")) return TOKEN_FALSE;

    return TOKEN_IDENTIFIER;

    #undef IsKeyword
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
           
            default: {
                size_t start = i;
                while (i++ < length) {  
                    if (input[i] == ' ' || input[i] == '\n' || input[i] == '\t' || input[i] == '(') break;
                }

                EmitToken(input, start, i, line, TokenAssignType(input, start, i), &token_list); 
                break;
            } 
        }
    }
    return token_list;
}

#include "../lang.h"

lang_tokenlist_t Lang_TokenListInit(size_t start_cap) {
    lang_tokenlist_t list = { 0 };
    list.capacity = start_cap;

    list.tokens = malloc(sizeof(lang_token_t) * start_cap);
    if (list.tokens == NULL) {
        printf("Oopsies, tokenlist couldn't allocate memory :(\n");
        exit(-1);
    }
    return list;
}

void Lang_TokenListAppend(lang_tokenlist_t* list, lang_token_t token) {
    list->tokens[list->length] = token;

    list->length++;
    if (list->length >= list->capacity) {
        lang_token_t* temp;

        list->capacity *= 2;
        temp = realloc(list->tokens, sizeof(lang_token_t)*list->capacity);

        if (temp == NULL) {
            printf("My bad gang. tokenlist ran out of memory to use :(\n");
            exit(-1);
        }
        list->tokens = temp;
    }
}

void Lang_EmitToken(char* code, size_t start, size_t end, size_t line, lang_tokentype_t type, lang_tokenlist_t* token_list) {
    lang_token_t new_token;
    new_token.type = type;
    new_token.line = line;
    new_token.length = end-start;
    new_token.start = code+start;

    Lang_TokenListAppend(token_list, new_token);
}

int Lang_IsDigit(char c) {
    if (c >= '0' && '9' >= c) return 1;
    return 0;
}

int Lang_IsNumber(char* source, size_t start, size_t end) {
    {int i; for (i=start; i<end; i++) {
        if (!Lang_IsDigit(source[i])) return 0;
    }}
    return 1;
}

int Lang_LexemeEquals(char* source, size_t start, size_t end, char* str, size_t str_length) {
    int length = end-start;
    if (length != str_length) return 0;

    {int i; for (i=0; i<length; i++) {
        if (source[start+i] != str[i]) return 0;
    }}

    return 1;
}

/* This is probably "fine" for perfomance. Atleast for now
   Kinda messed up tho */
lang_tokentype_t Lang_TokenAssignType(char* source, size_t start, size_t end) {
    /* Fucked up macro
       Got too verbose my bad gang
       We're doing this to avoid using strlen() which iterates through the string. Instead we get it's length at compile time.
       We also have to subtract the null terminator from the length */
    #define IsKeyword(str) Lang_LexemeEquals(source, start, end, str, sizeof(str)-1)
    
    /* Number */
    if (Lang_IsNumber(source, start, end)) return TOKEN_INT;

    /* Math */
    else if (IsKeyword("+")) return TOKEN_PLUS;
    else if (IsKeyword("-")) return TOKEN_MINUS;
    else if (IsKeyword("*")) return TOKEN_STAR;
    else if (IsKeyword("/")) return TOKEN_SLASH;
    else if (IsKeyword("%")) return TOKEN_PERCENT;

    /* Brackets */
    else if (IsKeyword("{")) return TOKEN_CURLY_L;
    else if (IsKeyword("}")) return TOKEN_CURLY_R;
    else if (IsKeyword("[")) return TOKEN_SQUARE_L;
    else if (IsKeyword("]")) return TOKEN_SQUARE_R;

    /* Logic */
    else if (IsKeyword("=")) return TOKEN_EQUAL;
    else if (IsKeyword("or")) return TOKEN_OR;
    else if (IsKeyword("and")) return TOKEN_AND;
    else if (IsKeyword("not")) return TOKEN_NOT;

    /* Stack manipulation */
    else if (IsKeyword("swap")) return TOKEN_SWAP;
    else if (IsKeyword("dup")) return TOKEN_DUP;
    else if (IsKeyword("drop")) return TOKEN_DROP;

    /* Control flow */
    else if (IsKeyword("if")) return TOKEN_IF;
    else if (IsKeyword("ifelse")) return TOKEN_IFELSE;
    else if (IsKeyword("for")) return TOKEN_FOR;
    else if (IsKeyword("while")) return TOKEN_WHILE;

    /* Booleans */
    else if (IsKeyword("true")) return TOKEN_TRUE;
    else if (IsKeyword("false")) return TOKEN_FALSE;

    return TOKEN_IDENTIFIER;

    #undef IsKeyword
}

lang_tokenlist_t Lang_Scan(char* input, size_t length) {
    lang_tokenlist_t token_list;
    size_t i;
    size_t line;

    token_list = Lang_TokenListInit(16);
    i = 0;
    line = 1;

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
                i++; /* Consume the ')' character */
                break;
            } 
           
            default: {
                size_t start;
                start = i;
                while (i++ < length) {  
                    if (input[i] == ' ' || input[i] == '\n' || input[i] == '\t' || input[i] == '(') break;
                }

                Lang_EmitToken(input, start, i, line, Lang_TokenAssignType(input, start, i), &token_list); 
                break;
            } 
        }
    }
    return token_list;
}

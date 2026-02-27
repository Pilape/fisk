#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ====================|| CONFIG || ======================
#define TOKEN_SIZE_LIMIT 32 // How long tokens can be in characters (excluding null terminator)

// =======================================================

char* ReadFile(char* path) {
    FILE* f = fopen(path, "r");
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        size_t length = ftell(f);
        fseek(f, 0, SEEK_SET);

        char* data = malloc(length+1);
        if (data != NULL) {
            fread(data, length, 1, f);
            data[length] = '\0';
            return data;
        }
    }
    return NULL;
}

typedef struct {
    enum {
        NUMBER,
        STRING,
        PRIMITIVE,
        LIST_START,
        LIST_END,
        FUNC_START,
        FUNC_END,

    } type;
    char lexeme[TOKEN_SIZE_LIMIT+1];
} token_t;

typedef struct {
    size_t capacity;
    size_t length;
    token_t* tokens;
} tokenlist_t;

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

tokenlist_t Tokenize(char* input, size_t length) {
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

void LangEval(char* str) {
    tokenlist_t token_list = Tokenize(str, strlen(str));

    for (int i=0; i<token_list.length; i++) {
        printf("%s\n", token_list.tokens[i].lexeme);
    }

    free(token_list.tokens);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage:\n\t./lang file/path.l\n");
        return 0;
    }

    char* input = ReadFile(argv[1]);
    LangEval(input);
    free(input);
    //printf("%s\n", ReadFile(argv[1])); 

    return 0;
}

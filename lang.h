#ifndef LANG_HEADER
#define LANG_HEADER
////////////////CONFIG////////////////

    ////////MEMORY/////////
    
    // The language does not use dynamic memory management.
    // Therefore, the memory used is decided at compile time.

    // The size of the data stack.
    #ifndef LANG_STACK_SIZE
        #define LANG_STACK_SIZE 256
    #endif

    // How many values/nodes are available.
    // Basically how big the languages memory arena is.
    #ifndef LANG_NODE_COUNT 
    #define LANG_NODE_COUNT 0x4000
    #endif

    // How many primitives the language can have, also used by "built in" primitives and the standard library.
    #ifndef LANG_PRIMITIVE_LIMIT
    #define LANG_PRIMITIVE_LIMIT 256
    #endif

    // How many functions and variables the language can have.
    #ifndef LANG_SYMBOL_LIMIT
    #define LANG_SYMBOL_LIMIT 512
    #endif

    // The max length of variable, function and primitive names
    #ifndef LANG_SYMBOL_LENGTH
    #define LANG_SYMBOL_LENGTH 32
    #endif

    ///////////////////////
    
//////////////////////////////////////

#define LANG_NULL ((void*)0)
#define LANG_TRUE 1
#define LANG_FALSE 0

#define LANG_ERROR_SIZE 512 // Size of error message buffer

////////////////STRUCTS/////////////////

struct lang_ctx;

enum lang_type {
    LANG_INT,
    LANG_CHAR,
    LANG_QUOT,
    LANG_PRIM,
};

struct lang_node;

struct lang_item {
    enum lang_type type;
    union {
        int integer;
        char character;
        struct lang_node* quotation;
        void (*primitive)(struct lang_ctx* ctx);
    } value;
};

struct lang_node {
    struct lang_item item;
    char allocated; 

    struct lang_node* next;
};

struct lang_primitive {
    char name[LANG_SYMBOL_LENGTH];
    void (*c_func)(struct lang_ctx* ctx);
};

struct lang_symbol {
    char name[LANG_SYMBOL_LENGTH];
    struct lang_node* value;
};

enum lang_state {
    LANG_OK,
    LANG_ERROR,
};

struct lang_ctx {
    struct lang_node nodes[LANG_NODE_COUNT];

    struct lang_item stack[LANG_STACK_SIZE];
    int stack_ptr;

    struct lang_primitive primitives[LANG_PRIMITIVE_LIMIT];
    int primitive_count;

    struct lang_symbol symbols[LANG_SYMBOL_LIMIT];
    int symbol_count;

    char error_msg[LANG_ERROR_SIZE+1]; // +1 for '\0'
    enum lang_state state;
};

struct lang_scanner {
    unsigned int start, current, line, input_len;
    char* input;
};

////////////////////////////////////////

struct lang_node* Lang_AllocateNode(struct lang_ctx* ctx);
struct lang_token Lang_Scan(struct lang_scanner* scanner, struct lang_ctx* ctx);

#endif // LANG_HEADER
#ifdef LANG_IMPLEMENTATION
#undef LANG_IMPLEMENTATION

struct lang_node* Lang_AllocateNode(struct lang_ctx* ctx) {
    for (unsigned int i=0; i<LANG_NODE_COUNT; i++) {
        if (ctx->nodes[i].allocated == LANG_FALSE) {
            ctx->nodes[i].allocated = LANG_TRUE;
            return &(ctx->nodes[i]);
        } 
    }
    return LANG_NULL;
}

//////// NUMBER ////////

char Lang_IsDigit(char c) {
    if (c <'0' || c > '9') return 0;
    return 1;
}

////////////////////////

/////// SCANNER ///////

unsigned int Lang_Strlen(char* str) {
    unsigned int len = 0;
    while (str[len] != '\0') {
        len++;
    }

    return len;
}

void Lang_Error(char* msg, struct lang_ctx* ctx) {
    ctx->state = LANG_ERROR;
    
    int str_len = Lang_Strlen(msg);
    if (str_len > LANG_ERROR_SIZE) str_len = LANG_ERROR_SIZE;

    for (int i=0; i<str_len; i++) {
        ctx->error_msg[i] = msg[i];
    }

    ctx->error_msg[str_len] = '\0';
}

enum lang_token_type {
    LANG_TOKEN_INT,
    LANG_TOKEN_STR,
    LANG_TOKEN_SYMBOL,

    LANG_TOKEN_CURLY_L,
    LANG_TOKEN_CURLY_R,

    LANG_TOKEN_NONE,
};

struct lang_token {
    enum lang_token_type type;
    unsigned int start, length, line;
};

#define LANG_TOKEN(token_type) ((struct lang_token){.type=(token_type), .line=scanner->line, .start=scanner->start, .length=scanner->current-scanner->start})

struct lang_token Lang_Scan(struct lang_scanner* scanner, struct lang_ctx* ctx) {
    while (1) {
        scanner->start = scanner->current;
        if (scanner->current >= scanner->input_len) {
            return LANG_TOKEN(LANG_TOKEN_NONE);
        }

        switch (scanner->input[scanner->current]) {
            case '\n':
                scanner->line++;
            case ' ':
            case '\t':
                scanner->current++;
                break;

            case '(':
                while (scanner->current++ < scanner->input_len) {
                    if (scanner->input[scanner->current] == ')') {
                        scanner->current++; // Consume ')'
                        break;
                    }
                }
                if (scanner->current >= scanner->input_len) {
                    Lang_Error("[ERROR]: Eternal comment of doom", ctx);
                    return LANG_TOKEN(LANG_TOKEN_NONE);
                }
                break;

            case '{':
                scanner->current++;
                return LANG_TOKEN(LANG_TOKEN_CURLY_L);
                break;
            case '}':
                scanner->current++;
                return LANG_TOKEN(LANG_TOKEN_CURLY_R);
                break;

            default: {
                enum lang_token_type symbol_type = (scanner->input[scanner->current] == '-' || Lang_IsDigit(scanner->input[scanner->current])) ? LANG_TOKEN_INT : LANG_TOKEN_SYMBOL;

                while (scanner->current++ < scanner->input_len) {
                    switch (scanner->input[scanner->current]) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '{':
                        case '}':
                        case '(':
                            goto __lang_scanner_goto_escape__; // Evil goto of doom
                            break;
                    }
                    if (!Lang_IsDigit(scanner->input[scanner->current])) symbol_type = LANG_TOKEN_SYMBOL;
                }

                __lang_scanner_goto_escape__:
                    if (scanner->current-scanner->start == 1 && scanner->input[scanner->start] == '-') symbol_type = LANG_TOKEN_SYMBOL; // '-' is not a number (crazy, i know)
                    return LANG_TOKEN(symbol_type);
                break;
            }
        }
    }
}

#undef LANG_TOKEN



///////////////////////


////// CODEGEN ///////



//////////////////////


#endif // LANG_IMPLEMENTATION

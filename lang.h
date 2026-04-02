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

struct lang_ctx {
    struct lang_node nodes[LANG_NODE_COUNT];

    struct lang_item stack[LANG_STACK_SIZE];
    int stack_ptr;

    struct lang_primitive primitives[LANG_PRIMITIVE_LIMIT];
    int primitive_count;

    struct lang_symbol symbols[LANG_SYMBOL_LIMIT];
    int symbol_count;
};

////////////////////////////////////////

struct lang_node* Lang_AllocateNode(struct lang_ctx* ctx);

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

enum lang_token_type {
    LANG_TOKEN_INT,
    LANG_TOKEN_STR,
    LANG_TOKEN_SYMBOL,

    LANG_TOKEN_CURLY_L,
    LANG_TOKEN_CURLY_R,

    LANG_TOKEN_SQUARE_L,
    LANG_TOKEN_SQUARE_R,

    LANG_TOKEN_NONE,
};

struct lang_token {
    enum lang_token_type type;
    unsigned int start, length, line;
};

struct lang_scanner {
    unsigned int start, current, line, input_len;
    char* input;
};

#define LANG_TOKEN(token_type) ((struct lang_token){.type=(token_type), .line=scanner->line, .start=scanner->start, .length=scanner->current-scanner->start})

struct lang_token Lang_Scan(struct lang_scanner* scanner, struct lang_ctx* ctx) {
    scanner->start = scanner->current;

    while (scanner->current < scanner->input_len) {

        switch (scanner->input[scanner->current]) {
            case '\n':
                scanner->line++;
            case ' ':
            case '\t':
                scanner->current++;
                break;

            case '(':
                while (scanner->current++ < scanner->input_len) {
                    if (scanner->input[scanner->current] == '\n') scanner->line++;
                    if (scanner->input[scanner->current] == ')') break;
                }
                scanner->current++; // Skip final ')'
                break;

            case '"': {
                unsigned int new_line = scanner->line; // Multiline string tokens have their line numbers as the line they start in.
                while (scanner->current++ < scanner->input_len) {
                    if (scanner->input[scanner->current] == '\n') new_line++;
                    if (scanner->input[scanner->current] == '"') break;
                }
                scanner->current++;
                return LANG_TOKEN(LANG_TOKEN_STR);
                scanner->line = new_line;
                break;
            }

            case '#':
                while (scanner->current++ < scanner->input_len) {
                    if (!Lang_IsDigit(scanner->input[scanner->current])) break;
                }
                return LANG_TOKEN(LANG_TOKEN_INT);
                break;

            case '{': return LANG_TOKEN(LANG_TOKEN_CURLY_L); break;
            case '}': return LANG_TOKEN(LANG_TOKEN_CURLY_R); break;

            case '[': return LANG_TOKEN(LANG_TOKEN_SQUARE_L); break;
            case ']': return LANG_TOKEN(LANG_TOKEN_SQUARE_R); break;

            default: {
                while (scanner->current++ < scanner->input_len) {
                    switch (scanner->input[scanner->current]) {
                        case '\n': 
                        case ' ': 
                        case '\t': 
                        case '(':
                        case '{': 
                        case '}': 
                        case '[': 
                        case ']':
                        case '"':
                            // Evil goto of doom
                            goto _lang_goto_scanner_escape;
                            break;

                        default: 
                            break;
                    }
                }
                _lang_goto_scanner_escape:
                return LANG_TOKEN(LANG_TOKEN_SYMBOL);
                break;
            }
                
        }
    }

    return LANG_TOKEN(LANG_TOKEN_NONE);
}
#undef LANG_TOKEN



///////////////////////


////// CODEGEN ///////



//////////////////////


#endif // LANG_IMPLEMENTATION

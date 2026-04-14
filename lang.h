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

#define LANG_ERROR_SIZE 512 // Size of error message buffer

////////////////STRUCTS/////////////////

struct lang_ctx;

enum lang_type {
    LANG_NIL,
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

void Lang_Error(char* msg, struct lang_ctx* ctx);
struct lang_node* Lang_AllocateNode(struct lang_ctx* ctx);
struct lang_token Lang_Scan(struct lang_scanner* scanner, struct lang_ctx* ctx);
void Lang_Eval(char* input, unsigned int input_len, struct lang_ctx* ctx);

#endif // LANG_HEADER
#ifdef LANG_IMPLEMENTATION
#undef LANG_IMPLEMENTATION

struct lang_node* Lang_AllocateNode(struct lang_ctx* ctx) {
    puts("allocating");
    for (unsigned int i=0; i<LANG_NODE_COUNT; i++) {
        if (ctx->nodes[i].allocated == 0) {
            puts("did it!");
            ctx->nodes[i].allocated = 1;
            return &(ctx->nodes[i]);
        } 
    }
    puts("failed");
    Lang_Error("[ERROR]: Out of memory", ctx);
    return LANG_NULL;
}

//////// NUMBER ////////

char Lang_IsDigit(char c) {
    if (c <'0' || c > '9') return 0;
    return 1;
}

int Lang_StrToInt(char* str, unsigned int strlen) {
    int num = 0;

    for (int i=0; i<strlen; i++) {
        char digit = str[i] - 48;
        num = num * 10 + digit;
    }

    return num;
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

            case '"':
                while (scanner->current++ < scanner->input_len) {
                    if (scanner->input[scanner->current] == '\n') scanner->line++;
                    if (scanner->input[scanner->current] == '"') {
                        scanner->current++; // Consume last '"'
                        break;
                    }
                }
                if (scanner->current >= scanner->input_len) {
                    Lang_Error("[ERROR]: Unterminated string", ctx);
                    return LANG_TOKEN(LANG_TOKEN_NONE);
                }
                return LANG_TOKEN(LANG_TOKEN_STR);
                break;

            case '(':
                while (scanner->current++ < scanner->input_len) {
                    if (scanner->input[scanner->current] == ')') {
                        scanner->current++; // Consume ')'
                        break;
                    }
                }
                if (scanner->current >= scanner->input_len) {
                    Lang_Error("[ERROR]: Unterminated comment", ctx);
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
                        case '"':
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

struct lang_item Lang_TokenToItem(struct lang_token token, struct lang_scanner* scanner, struct lang_ctx* ctx) {
    struct lang_item item = { 0 };
    item.type = LANG_NIL;

    switch (token.type) {
        case LANG_TOKEN_INT:
            item.type = LANG_INT;
            item.value.integer = Lang_StrToInt(&scanner->input[token.start], token.length);
            break;

        case LANG_TOKEN_STR: {
            /*struct lang_node* str_head = Lang_AllocateNode(ctx);
            if (str_head == LANG_NULL) return item;
            // First character
            str_head->item.type = LANG_CHAR;
            str_head->item.value.character = scanner->input[token.start+1];
            
            struct lang_node* current = str_head;
            for (int i=2; i<token.length-1; i++) { // Skip the quotation marks and first char
                struct lang_node* temp = Lang_AllocateNode(ctx);
                if (str_head == LANG_NULL) return item;

                temp->item.type = LANG_CHAR;
                temp->item.value.character = scanner->input[token.start+i];

                current->next = temp;
                current = temp;
            }*/

            item.type = LANG_QUOT;
            struct lang_node* str_head = LANG_NULL;

            if (token.length == 0) break;

            struct lang_node* current = LANG_NULL;
            for (int i=1; i<token.length-1; i++) { // Ignore quotation marks
                current = Lang_AllocateNode(ctx);
                if (current == LANG_NULL) return item; 

                current->item.type = LANG_CHAR;
                current->item.value.character = scanner->input[token.start+i];

                if (i == 1) str_head = current;
            }
            item.value.quotation = str_head;
            break;
        }

        default:
            Lang_Error("[ERROR]: Unknown token type", ctx);
            break;
    }

    return item;
}

void Lang_Push(struct lang_item item, struct lang_ctx* ctx) {
    ctx->stack[ctx->stack_ptr++] = item;
}

void Lang_ExecuteItem(struct lang_item item, struct lang_ctx* ctx) {
    switch (item.type) {
        default:
            Lang_Push(item, ctx);
            break;
            
    }
}

////// RUNTIME ///////
void Lang_Eval(char* input, unsigned int input_len, struct lang_ctx* ctx) {
    struct lang_scanner scanner = {
        .line = 1,
        .start = 0,
        .current = 0,
        .input_len = input_len,
        .input = input,
    };

    while (1) {
        struct lang_token token = Lang_Scan(&scanner, ctx);
        if (token.type == LANG_TOKEN_NONE) break;
        
        struct lang_item item = Lang_TokenToItem(token, &scanner, ctx);
        if (ctx->state != LANG_OK) return;
        Lang_ExecuteItem(item, ctx);

    }
    if (ctx->state == LANG_ERROR) {
        return;
    }

}


//////////////////////


#endif // LANG_IMPLEMENTATION

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
void Lang_AddPrimitive(void (*func)(struct lang_ctx* ctx), char* name, struct lang_ctx* ctx);
void Lang_Push(struct lang_item item, struct lang_ctx* ctx);
struct lang_item Lang_Pop(struct lang_ctx* ctx);
void Lang_Eval(char* input, unsigned int input_len, struct lang_ctx* ctx);

#endif // LANG_HEADER
#ifdef LANG_IMPLEMENTATION
#undef LANG_IMPLEMENTATION

struct lang_node* Lang_AllocateNode(struct lang_ctx* ctx) {
    for (unsigned int i=0; i<LANG_NODE_COUNT; i++) {
        if (ctx->nodes[i].allocated == 0) {
            ctx->nodes[i].allocated = 1;

            ctx->nodes[i].next = LANG_NULL;
            ctx->nodes[i].item.type = LANG_NIL;

            return &(ctx->nodes[i]);
        } 
    }
    Lang_Error("[ERROR]: Out of memory", ctx);
    return LANG_NULL;
}

//////// NUMBER ////////

char Lang_IsDigit(char c) {
    if (c <'0' || c > '9') return 0;
    return 1;
}

int Lang_StrToInt(char* str, unsigned int strlen) {
    char is_negative = str[0] == '-';

    int num = 0;

    int start_index = is_negative;
    for (int i=start_index; i<strlen; i++) {
        char digit = str[i] - 48;
        num = num * 10 + digit;
    }

    if (is_negative) num *= -1;

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

char Lang_StrIsEqual(char* str1, char* str2) {
    int i = 0;
    while (1) {
        if (str1[i] != str2[i]) return 0;
        if (str1[i] == '\0' || str2[i] == '\0') break;
        i++;
    }

    return 1;
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

void Lang_AddPrimitive(void (*func)(struct lang_ctx* ctx), char* name, struct lang_ctx* ctx) {
    if (ctx->primitive_count >= LANG_PRIMITIVE_LIMIT) {
        Lang_Error("[ERROR]: Primtive limit reached", ctx);
        return;
    } 

    for (int i=0; i<ctx->primitive_count; i++) {
        if (Lang_StrIsEqual(name, ctx->primitives[i].name)) {
            Lang_Error("[ERROR]: Primitive name is already taken", ctx);
            return;
        }
    }
 
    struct lang_primitive* primitive = &ctx->primitives[ctx->primitive_count];

    int name_length = Lang_Strlen(name);
    if (name_length >= LANG_PRIMITIVE_LIMIT) {
        Lang_Error("[ERROR]: Primitive name is too long", ctx);
        return;
    }

    // Copy string over (TODO: Maybe make this a seperate function. Too lazy to do it rn) (Kinda small so i might not need to...)
    for (int i=0; i<name_length; i++) {
        primitive->name[i] = name[i];
    }

    primitive->c_func = func;

    ctx->primitive_count++;
}

// What
void (*Lang_GetPrimitiveFunc(char* name, struct lang_ctx* ctx))(struct lang_ctx*) {
    for (int i=0; i<ctx->primitive_count; i++) {
        if (Lang_StrIsEqual(name, ctx->primitives[i].name)) return ctx->primitives[i].c_func;
    }

    return LANG_NULL;
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

static inline void Lang_StrToItem(struct lang_token token, struct lang_scanner* scanner, struct lang_item* item, struct lang_ctx* ctx) {
    item->type = LANG_QUOT;
    if (token.length == 0) return;

    // First character
    struct lang_node* current = Lang_AllocateNode(ctx);
    if (current == LANG_NULL) return;
    current->item.type = LANG_CHAR;
    current->item.value.character = scanner->input[token.start+1]; 
    
    item->value.quotation = current;

    for (int i=2; i<token.length-1; i++) { // Ignore quotation marks and first char
        current->next = Lang_AllocateNode(ctx);
        if (current->next == LANG_NULL) break;

        current = current->next;

        current->item.type = LANG_CHAR;
        current->item.value.character = scanner->input[token.start+i]; 
    }
}

struct lang_item Lang_TokenToItem(struct lang_token token, struct lang_scanner* scanner, struct lang_ctx* ctx) {
    struct lang_item item = { 0 };
    item.type = LANG_NIL;

    switch (token.type) {
        case LANG_TOKEN_INT:
            item.type = LANG_INT;
            item.value.integer = Lang_StrToInt(&scanner->input[token.start], token.length);
            break;

        case LANG_TOKEN_STR:
            Lang_StrToItem(token, scanner, &item, ctx);
            break;

        case LANG_TOKEN_SYMBOL: {
            // Convert lexeme to string
            char symbol[LANG_SYMBOL_LENGTH];
            for (int i=0; i<token.length; i++) {
                symbol[i] = scanner->input[i+token.start];
            }
            symbol[token.length] = '\0';

            void (*func)(struct lang_ctx* ctx) = Lang_GetPrimitiveFunc(symbol, ctx);
            if (func == LANG_NULL) {
                Lang_Error("[ERROR]: It's so over", ctx);
                break;
            }

            item.type = LANG_PRIM;
            item.value.primitive = func;
            break;
        }

        default:
            // Should not be reachable (Keeping it just in case)
            Lang_Error("[ERROR]: Unknown token type", ctx);
            break;
    }

    return item;
}

void Lang_Push(struct lang_item item, struct lang_ctx* ctx) {
    ctx->stack[ctx->stack_ptr++] = item;
}

struct lang_item Lang_Pop(struct lang_ctx* ctx) {
    return ctx->stack[--ctx->stack_ptr];
}

void Lang_ExecuteItem(struct lang_item item, struct lang_ctx* ctx) {
    switch (item.type) {
        case LANG_PRIM:
            item.value.primitive(ctx);
            break;

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

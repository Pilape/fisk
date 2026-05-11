#ifndef FISK_HEADER
#define FISK_HEADER
////////////////CONFIG////////////////

    ////////MEMORY/////////
    
    // The language does not use dynamic memory management.
    // Therefore, the memory used is decided at compile time.

    // The size of the data stack.
    #ifndef FISK_STACK_SIZE
        #define FISK_STACK_SIZE 256
    #endif

    // How many values/nodes are available.
    // Basically how big the languages memory arena is.
    #ifndef FISK_NODE_COUNT 
    #define FISK_NODE_COUNT 0x4000
    #endif

    // How many primitives the language can have, also used by "built in" primitives and the standard library.
    #ifndef FISK_PRIMITIVE_LIMIT
    #define FISK_PRIMITIVE_LIMIT 256
    #endif

    // How many functions and variables the language can have.
    #ifndef FISK_SYMBOL_LIMIT
    #define FISK_SYMBOL_LIMIT 512
    #endif

    // The max length of variable, function and primitive names
    #ifndef FISK_SYMBOL_LENGTH
    #define FISK_SYMBOL_LENGTH 32
    #endif

    ///////////////////////
    
//////////////////////////////////////

#define FISK_NULL ((void*)0)

#define FISK_ERROR_SIZE 512 // Size of error message buffer

////////////////STRUCTS/////////////////

struct fisk_ctx;

enum fisk_type {
    FISK_NIL,
    FISK_INT,
    FISK_CHAR,
    FISK_QUOT,
    FISK_PRIM,
};

struct fisk_node;

struct fisk_item {
    enum fisk_type type;
    union {
        int integer;
        char character;
        struct fisk_node* quotation;
        void (*primitive)(struct fisk_ctx* ctx);
    } value;
};
struct fisk_node {
    struct fisk_item item;
    char allocated; 

    struct fisk_node* next;
};

struct fisk_primitive {
    char name[FISK_SYMBOL_LENGTH];
    void (*c_func)(struct fisk_ctx* ctx);
};

struct fisk_symbol {
    char name[FISK_SYMBOL_LENGTH];
    struct fisk_node* value;
};

enum fisk_state {
    FISK_OK,
    FISK_ERROR,
};

struct fisk_ctx {
    struct fisk_node nodes[FISK_NODE_COUNT];

    struct fisk_item stack[FISK_STACK_SIZE];
    int stack_ptr;

    struct fisk_primitive primitives[FISK_PRIMITIVE_LIMIT];
    int primitive_count;

    struct fisk_symbol symbols[FISK_SYMBOL_LIMIT];
    int symbol_count;

    char error_msg[FISK_ERROR_SIZE+1]; // +1 for '\0'
    enum fisk_state state;
};

struct fisk_scanner {
    unsigned int start, current, line, input_len;
    char* input;
};

////////////////////////////////////////

void Fisk_Error(char* msg, struct fisk_ctx* ctx);
struct fisk_node* Fisk_AllocateNode(struct fisk_ctx* ctx);
struct fisk_token Fisk_Scan(struct fisk_scanner* scanner, struct fisk_ctx* ctx);
void Fisk_AddPrimitive(void (*func)(struct fisk_ctx* ctx), char* name, struct fisk_ctx* ctx);
void Fisk_Push(struct fisk_item item, struct fisk_ctx* ctx);
struct fisk_item Fisk_Pop(struct fisk_ctx* ctx);
void Fisk_Eval(char* input, unsigned int input_len, struct fisk_ctx* ctx);

#endif // FISK_HEADER
#ifdef FISK_IMPLEMENTATION
#undef FISK_IMPLEMENTATION

struct fisk_node* Fisk_AllocateNode(struct fisk_ctx* ctx) {
    for (unsigned int i=0; i<FISK_NODE_COUNT; i++) {
        if (ctx->nodes[i].allocated == 0) {
            ctx->nodes[i].allocated = 1;

            ctx->nodes[i].next = FISK_NULL;
            ctx->nodes[i].item.type = FISK_NIL;

            return &(ctx->nodes[i]);
        } 
    }
    Fisk_Error("[ERROR]: Out of memory", ctx);
    return FISK_NULL;
}

//////// NUMBER ////////

char Fisk_IsDigit(char c) {
    if (c <'0' || c > '9') return 0;
    return 1;
}

int Fisk_StrToInt(char* str, unsigned int strlen) {
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

unsigned int Fisk_Strlen(char* str) {
    unsigned int len = 0;
    while (str[len] != '\0') {
        len++;
    }

    return len;
}

char Fisk_StrIsEqual(char* str1, char* str2) {
    int i = 0;
    while (1) {
        if (str1[i] != str2[i]) return 0;
        if (str1[i] == '\0' || str2[i] == '\0') break;
        i++;
    }

    return 1;
}

void Fisk_Error(char* msg, struct fisk_ctx* ctx) {
    ctx->state = FISK_ERROR;
    
    int str_len = Fisk_Strlen(msg);
    if (str_len > FISK_ERROR_SIZE) str_len = FISK_ERROR_SIZE;

    for (int i=0; i<str_len; i++) {
        ctx->error_msg[i] = msg[i];
    }

    ctx->error_msg[str_len] = '\0';
}

void Fisk_AddPrimitive(void (*func)(struct fisk_ctx* ctx), char* name, struct fisk_ctx* ctx) {
    if (ctx->primitive_count >= FISK_PRIMITIVE_LIMIT) {
        Fisk_Error("[ERROR]: Primtive limit reached", ctx);
        return;
    } 

    for (int i=0; i<ctx->primitive_count; i++) {
        if (Fisk_StrIsEqual(name, ctx->primitives[i].name)) {
            Fisk_Error("[ERROR]: Primitive name is already taken", ctx);
            return;
        }
    }
 
    struct fisk_primitive* primitive = &ctx->primitives[ctx->primitive_count];

    int name_length = Fisk_Strlen(name);
    if (name_length >= FISK_PRIMITIVE_LIMIT) {
        Fisk_Error("[ERROR]: Primitive name is too long", ctx);
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
void (*Fisk_GetPrimitiveFunc(char* name, struct fisk_ctx* ctx))(struct fisk_ctx*) {
    for (int i=0; i<ctx->primitive_count; i++) {
        if (Fisk_StrIsEqual(name, ctx->primitives[i].name)) return ctx->primitives[i].c_func;
    }

    return FISK_NULL;
}

enum fisk_token_type {
    FISK_TOKEN_INT,
    FISK_TOKEN_STR,
    FISK_TOKEN_SYMBOL,

    FISK_TOKEN_CURLY_L,
    FISK_TOKEN_CURLY_R,

    FISK_TOKEN_NONE,
};

struct fisk_token {
    enum fisk_token_type type;
    unsigned int start, length, line;
};

#define FISK_TOKEN(token_type) ((struct fisk_token){.type=(token_type), .line=scanner->line, .start=scanner->start, .length=scanner->current-scanner->start})

struct fisk_token Fisk_Scan(struct fisk_scanner* scanner, struct fisk_ctx* ctx) {
    while (1) {
        scanner->start = scanner->current;
        if (scanner->current >= scanner->input_len) {
            return FISK_TOKEN(FISK_TOKEN_NONE);
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
                    Fisk_Error("[ERROR]: Unterminated string", ctx);
                    return FISK_TOKEN(FISK_TOKEN_NONE);
                }
                return FISK_TOKEN(FISK_TOKEN_STR);
                break;

            case '(':
                while (scanner->current++ < scanner->input_len) {
                    if (scanner->input[scanner->current] == ')') {
                        scanner->current++; // Consume ')'
                        break;
                    }
                }
                if (scanner->current >= scanner->input_len) {
                    Fisk_Error("[ERROR]: Unterminated comment", ctx);
                    return FISK_TOKEN(FISK_TOKEN_NONE);
                }
                break;

            case '{':
                scanner->current++;
                return FISK_TOKEN(FISK_TOKEN_CURLY_L);
                break;
            case '}':
                scanner->current++;
                return FISK_TOKEN(FISK_TOKEN_CURLY_R);
                break;

            default: {
                enum fisk_token_type symbol_type = (scanner->input[scanner->current] == '-' || Fisk_IsDigit(scanner->input[scanner->current])) ? FISK_TOKEN_INT : FISK_TOKEN_SYMBOL;

                while (scanner->current++ < scanner->input_len) {
                    switch (scanner->input[scanner->current]) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '{':
                        case '}':
                        case '(':
                        case '"':
                            goto __fisk_scanner_goto_escape__; // Evil goto of doom
                            break;
                    }
                    if (!Fisk_IsDigit(scanner->input[scanner->current])) symbol_type = FISK_TOKEN_SYMBOL;
                }

                __fisk_scanner_goto_escape__:
                    if (scanner->current-scanner->start == 1 && scanner->input[scanner->start] == '-') symbol_type = FISK_TOKEN_SYMBOL; // '-' is not a number (crazy, i know)
                    return FISK_TOKEN(symbol_type);
                break;
            }
        }
    }
}

#undef FISK_TOKEN



///////////////////////

static inline void Fisk_StrToItem(struct fisk_token token, struct fisk_scanner* scanner, struct fisk_item* item, struct fisk_ctx* ctx) {
    item->type = FISK_QUOT;
    if (token.length == 0) return;

    // First character
    struct fisk_node* current = Fisk_AllocateNode(ctx);
    if (current == FISK_NULL) return;
    current->item.type = FISK_CHAR;
    current->item.value.character = scanner->input[token.start+1]; 
    
    item->value.quotation = current;

    for (int i=2; i<token.length-1; i++) { // Ignore quotation marks and first char
        current->next = Fisk_AllocateNode(ctx);
        if (current->next == FISK_NULL) break;

        current = current->next;

        current->item.type = FISK_CHAR;
        current->item.value.character = scanner->input[token.start+i]; 
    }
}

struct fisk_item Fisk_TokenToItem(struct fisk_token token, struct fisk_scanner* scanner, struct fisk_ctx* ctx) {
    struct fisk_item item = { 0 };
    item.type = FISK_NIL;

    switch (token.type) {
        case FISK_TOKEN_INT:
            item.type = FISK_INT;
            item.value.integer = Fisk_StrToInt(&scanner->input[token.start], token.length);
            break;

        case FISK_TOKEN_STR:
            Fisk_StrToItem(token, scanner, &item, ctx);
            break;

        case FISK_TOKEN_SYMBOL: {
            // Convert lexeme to string
            char symbol[FISK_SYMBOL_LENGTH];
            for (int i=0; i<token.length; i++) {
                symbol[i] = scanner->input[i+token.start];
            }
            symbol[token.length] = '\0';

            void (*func)(struct fisk_ctx* ctx) = Fisk_GetPrimitiveFunc(symbol, ctx);
            if (func == FISK_NULL) {
                Fisk_Error("[ERROR]: It's so over", ctx);
                break;
            }

            item.type = FISK_PRIM;
            item.value.primitive = func;
            break;
        }

        default:
            // Should not be reachable (Keeping it just in case)
            Fisk_Error("[ERROR]: Unknown token type", ctx);
            break;
    }

    return item;
}

void Fisk_Push(struct fisk_item item, struct fisk_ctx* ctx) {
    ctx->stack[ctx->stack_ptr++] = item;
}

struct fisk_item Fisk_Pop(struct fisk_ctx* ctx) {
    return ctx->stack[--ctx->stack_ptr];
}

void Fisk_ExecuteItem(struct fisk_item item, struct fisk_ctx* ctx) {
    switch (item.type) {
        case FISK_PRIM:
            item.value.primitive(ctx);
            break;

        default:
            Fisk_Push(item, ctx);
            break;
            
    }
}

////// RUNTIME ///////
void Fisk_Eval(char* input, unsigned int input_len, struct fisk_ctx* ctx) {
    struct fisk_scanner scanner = {
        .line = 1,
        .start = 0,
        .current = 0,
        .input_len = input_len,
        .input = input,
    };

    while (1) {
        struct fisk_token token = Fisk_Scan(&scanner, ctx);
        if (token.type == FISK_TOKEN_NONE) break;
        
        struct fisk_item item = Fisk_TokenToItem(token, &scanner, ctx);
        if (ctx->state != FISK_OK) return;
        Fisk_ExecuteItem(item, ctx);

    }
    if (ctx->state == FISK_ERROR) {
        return;
    }

}


//////////////////////


#endif // FISK_IMPLEMENTATION

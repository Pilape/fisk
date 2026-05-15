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

////////////////STRUCTS/////////////////

struct fisk_ctx;

struct fisk_primitive {
    char name[FISK_SYMBOL_LENGTH];
    void (*c_func)(struct fisk_ctx* ctx);
};

struct fisk_symbol {
    char name[FISK_SYMBOL_LENGTH];
    struct fisk_node* value;
};

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
        struct fisk_primitive* primitive;
    } value;
};

struct fisk_node {
    struct fisk_item item;
    char allocated; 

    struct fisk_node* next;
};


enum fisk_status_code {
    FISK_OK,
    FISK_ERROR,
};


enum fisk_error_type {
    FISK_ERR_UNTERM_COMMENT,
    FISK_ERR_UNTERM_STRING,
    FISK_ERR_UNKOWN_WORD,

    FISK_ERR_OVERFLOW,
    FISK_ERR_UNDERFLOW,
    FISK_ERR_NO_MEMORY,

    FISK_ERR_TOO_MANY_PRIM,
    FISK_ERR_PRIM_TOO_LONG,
    FISK_ERR_PRIM_TAKEN,
};

const char* fisk_error_messages[] = {
    "Unterminated comment",
    "Unterminated string",
    "Unkown word",

    "Stack overflow",
    "Stack underflow",
    "Out of memory",

    "Max primitive count exceeded",
    "Primitive name is too long",
    "A primitive with this name is already in use",
};


struct fisk_ctx {
    struct fisk_node nodes[FISK_NODE_COUNT];

    struct fisk_item stack[FISK_STACK_SIZE];
    int stack_ptr;

    struct fisk_primitive primitives[FISK_PRIMITIVE_LIMIT];
    int primitive_count;

    struct fisk_symbol symbols[FISK_SYMBOL_LIMIT];
    int symbol_count;

    struct {
        enum fisk_status_code code;
        enum fisk_error_type message_index;
    } status;
};

////////////////////////////////////////

struct fisk_node* Fisk_AllocateNode(struct fisk_ctx* ctx);
void Fisk_AddPrimitive(void (*func)(struct fisk_ctx* ctx), char* name, struct fisk_ctx* ctx);
void Fisk_Push(struct fisk_item item, struct fisk_ctx* ctx);
struct fisk_item Fisk_Pop(struct fisk_ctx* ctx);
void Fisk_Eval(char* input, unsigned int input_len, struct fisk_ctx* ctx);

#endif // FISK_HEADER












#ifdef FISK_IMPLEMENTATION
#undef FISK_IMPLEMENTATION


//////////////////////////////////////////////////////////////////////////////////// HELPER / MISC /////////////////////////////////////////////////////////////////////////////


///////////////////////// NUMBER /////////////////////////////

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

//////////////////////////////////////////////////////////////

/////////////////////////////// STRING //////////////////////////////

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
//////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////// ERROR ////////////////////////////////////////

#define FISK_ERROR(type, ctx_ptr) do {       \
    (ctx_ptr)->status.code = FISK_ERROR;     \
    (ctx_ptr)-> status.message_index = type; \
} while (0)                                  \

//////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////// SCANNER ////////////////////////////////////////////////////////////////////////////////////////////
struct fisk_scanner {
    unsigned int start, current, line, input_len;
    char* input;
};

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
                    FISK_ERROR(FISK_ERR_UNTERM_STRING, ctx);
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
                    FISK_ERROR(FISK_ERR_UNTERM_COMMENT, ctx);
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////













struct fisk_node* Fisk_AllocateNode(struct fisk_ctx* ctx) {
    for (unsigned int i=0; i<FISK_NODE_COUNT; i++) {
        if (ctx->nodes[i].allocated == 0) {
            ctx->nodes[i].allocated = 1;

            ctx->nodes[i].next = FISK_NULL;
            ctx->nodes[i].item.type = FISK_NIL;

            return &(ctx->nodes[i]);
        } 
    }
    FISK_ERROR(FISK_ERR_NO_MEMORY, ctx);
    return FISK_NULL;
}

void Fisk_AddPrimitive(void (*func)(struct fisk_ctx* ctx), char* name, struct fisk_ctx* ctx) {
    if (ctx->primitive_count >= FISK_PRIMITIVE_LIMIT) {
        FISK_ERROR(FISK_ERR_TOO_MANY_PRIM, ctx);
        return;
    } 

    for (int i=0; i<ctx->primitive_count; i++) {
        if (Fisk_StrIsEqual(name, ctx->primitives[i].name)) {
            FISK_ERROR(FISK_ERR_PRIM_TAKEN, ctx);
            return;
        }
    }
 
    struct fisk_primitive* primitive = &ctx->primitives[ctx->primitive_count];

    int name_length = Fisk_Strlen(name);
    if (name_length >= FISK_PRIMITIVE_LIMIT) {
        FISK_ERROR(FISK_ERR_PRIM_TOO_LONG, ctx);
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
/*void (*Fisk_GetPrimitiveFunc(char* name, struct fisk_ctx* ctx))(struct fisk_ctx*) {
    for (int i=0; i<ctx->primitive_count; i++) {
        if (Fisk_StrIsEqual(name, ctx->primitives[i].name)) return ctx->primitives[i].c_func;
    }

    return FISK_NULL;
}*/
struct fisk_primitive* Fisk_GetPrimitive(char* name, struct fisk_ctx* ctx) {
    for (int i=0; i<ctx->primitive_count; i++) {
        if (Fisk_StrIsEqual(name, ctx->primitives[i].name)) return &ctx->primitives[i];
    }

    return FISK_NULL;
}
///////////////////////

// TODO: Store tail in quotation, for faster appending (it's O(n) right now :(  )
void Fisk_QuotationAppend(struct fisk_node** quotation, struct fisk_item item, struct fisk_ctx* ctx) {
    if (*quotation == FISK_NULL) {
        *quotation = Fisk_AllocateNode(ctx);
        if (*quotation == FISK_NULL) return;

        (*quotation)->item = item;
        return;
    }
    
    struct fisk_node* current = *quotation;

    while (current->next != FISK_NULL) { current = current->next; }

    current->next = Fisk_AllocateNode(ctx);
    if (current->next == FISK_NULL) return;

    current->next->item = item;
}

static inline void Fisk_StrToItem(struct fisk_token token, struct fisk_scanner* scanner, struct fisk_item* item, struct fisk_ctx* ctx) {
    item->type = FISK_QUOT;
    item->value.quotation = FISK_NULL;

    // Ignore the quotation marks
    for (int i=1; i<token.length-1; i++) {
        struct fisk_item character = {
            .type = FISK_CHAR,
            .value = scanner->input[i],
        };
    
        Fisk_QuotationAppend(&item->value.quotation, character, ctx);
    }

    //if (token.length == 0) return;

    /*// First character
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
    }*/
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

            struct fisk_primitive* primitive = Fisk_GetPrimitive(symbol, ctx);
            if (primitive == FISK_NULL) {
                FISK_ERROR(FISK_ERR_UNKOWN_WORD, ctx);
                break;
            }

            item.type = FISK_PRIM;
            item.value.primitive = primitive;
            break;
        }

        case FISK_TOKEN_CURLY_L: {
            item.type = FISK_QUOT;
            item.value.quotation = FISK_NULL;

            /*struct fisk_node** current = &item.value.quotation;
            struct fisk_node* prev = FISK_NULL;*/
            
            //*current = Fisk_AllocateNode(ctx);
            //if (*current == FISK_NULL) break;
            
            struct fisk_token token = Fisk_Scan(scanner, ctx);

            while (token.type != FISK_TOKEN_CURLY_R) {
                /* *current = Fisk_AllocateNode(ctx);
                if (*current == FISK_NULL) return item; */

                // (*current)->item = Fisk_TokenToItem(token, scanner, ctx);
                
                Fisk_QuotationAppend(&item.value.quotation, Fisk_TokenToItem(token, scanner, ctx), ctx);
                /*prev = *current;
                *current = (*current)->next;
                printf("p %p\n", prev);
                prev->next = *current;*/

                token = Fisk_Scan(scanner, ctx);
            }

            break;
        }
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
            item.value.primitive->c_func(ctx);
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
        if (ctx->status.code != FISK_OK) return;
        Fisk_ExecuteItem(item, ctx);

    }
    if (ctx->status.code == FISK_ERROR) {
        return;
    }

}


//////////////////////


#undef FISK_ERROR
#endif // FISK_IMPLEMENTATION

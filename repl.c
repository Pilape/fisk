#include <stdio.h>
#include <string.h>

#define LANG_IMPLEMENTATION
#define LANG_NODE_COUNT 32
#include "lang.h"

void PrintItem(struct lang_item item, struct lang_ctx* ctx);

void PrintQuotation(struct lang_item quot, struct lang_ctx* ctx) {
    printf(" {");
    struct lang_node* current = quot.value.quotation;
    while (current != LANG_NULL) {
        PrintItem(current->item, ctx);
        current = current->next;
    }
    printf(" }");
}

void PrintItem(struct lang_item item, struct lang_ctx* ctx) {
    switch (item.type) {
        case LANG_INT:
            printf(" %d", item.value.integer);
            break;

        case LANG_CHAR:
            printf(" '%c'", item.value.character);
            break;

        case LANG_QUOT:
            PrintQuotation(item, ctx);
            break;

        case LANG_PRIM:
            printf(" [PRIMITIVE]");
            break;

        case LANG_NIL:
            printf(" nil");
            break;
    }
}

int main() {
    struct lang_ctx lang = { 0 };

    puts("Lang repl");
    puts("ctrl+c to exit");

    while (1) {
        fputs("$ ", stdout);
        
        char buf[256];
        fgets(buf, sizeof(buf), stdin);
        
        Lang_Eval(buf, strlen(buf), &lang);
        if (lang.state != LANG_OK) {
            puts(lang.error_msg);
            return 1;
        }
        
        puts("ok");

        printf("(Stack):");
        for (int i=0; i<lang.stack_ptr; i++) {
            PrintItem(lang.stack[i], &lang);
        }
        puts("");
    }

    return 0;
}

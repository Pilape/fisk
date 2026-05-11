#include <stdio.h>
#include <string.h>

#define FISK_IMPLEMENTATION
#define FISK_NODE_COUNT 32
#include "fisk.h"

void PrintItem(struct fisk_item item, struct fisk_ctx* ctx);

void PrintQuotation(struct fisk_item quot, struct fisk_ctx* ctx) {
    printf(" {");
    struct fisk_node* current = quot.value.quotation;
    while (current != FISK_NULL) {
        PrintItem(current->item, ctx);
        current = current->next;
    }
    printf(" }");
}

void PrintItem(struct fisk_item item, struct fisk_ctx* ctx) {
    switch (item.type) {
        case FISK_INT:
            printf(" %d", item.value.integer);
            break;

        case FISK_CHAR:
            printf(" '%c'", item.value.character);
            break;

        case FISK_QUOT:
            PrintQuotation(item, ctx);
            break;

        case FISK_PRIM:
            printf(" [PRIMITIVE]");
            break;

        case FISK_NIL:
            printf(" nil");
            break;
    }
}

void Primitive_PrintSomethingFunny(struct fisk_ctx* ctx) {
    puts("Something funny");
}

void Primitive_Add(struct fisk_ctx* ctx) {
    struct fisk_item b = Fisk_Pop(ctx);
    struct fisk_item a = Fisk_Pop(ctx);

    a.value.integer += b.value.integer;

    Fisk_Push(a, ctx);
}

int main() {
    struct fisk_ctx fisk = { 0 };

    Fisk_AddPrimitive(&Primitive_PrintSomethingFunny, "funny", &fisk);
    Fisk_AddPrimitive(&Primitive_Add, "+", &fisk);
    if (fisk.state != FISK_OK) {
        puts(fisk.error_msg);
        return 1;
    }

    puts("== FISK ==");
    puts("ctrl+c to exit");

    while (1) {
        fputs("><> ", stdout);
        
        char buf[256];
        fgets(buf, sizeof(buf), stdin);
        
        Fisk_Eval(buf, strlen(buf), &fisk);
        if (fisk.state != FISK_OK) {
            puts(fisk.error_msg);
            return 1;
        }
        
        puts("ok");

        printf("(Stack):");
        for (int i=0; i<fisk.stack_ptr; i++) {
            PrintItem(fisk.stack[i], &fisk);
        }
        puts("");
    }

    return 0;
}

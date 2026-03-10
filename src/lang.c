#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lang.h"

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

void LangEval(char* str, lang_t* ctx) {
    tokenlist_t token_list = Scan(str, strlen(str));
    GenerateBytecode(token_list, &ctx->vm);
    /*for (int i=0; i<token_list.length; i++) {
        token_t token = token_list.tokens[i];
        printf("Line: %2d | Type: %2d | ", token.line, token.type);
        for (int j=0; j<token.length; j++) {
            printf("%c", token.start[j]);
        }
        printf("\n");
    }*/

    free(token_list.tokens);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage:\n\t./lang file/path.l\n");
        return 0;
    }

    char* input = ReadFile(argv[1]);
    lang_t lang = { 0 };
    LangEval(input, &lang);
    free(input);
    //printf("%s\n", ReadFile(argv[1])); 

    return 0;
}

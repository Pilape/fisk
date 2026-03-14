#include "lang.h"
#include <string.h>

char* ReadFile(char* path) {
    FILE* f;
    f = fopen(path, "r");
    if (f != NULL) {
        size_t length;
        char* data;

        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);

        data = malloc(length+1);
        if (data != NULL) {
            fread(data, length, 1, f);
            data[length] = '\0';
            return data;
        }
    }
    return NULL;
}

void Lang_Eval(char* str, lang_state_t* ctx) {
    lang_tokenlist_t token_list;
    token_list = Lang_Scan(str, strlen(str));
    Lang_GenerateBytecode(token_list, &ctx->vm);
    {
        int i;
        for (i=0; i<token_list.length; i++) {
            lang_token_t token;
            token = token_list.tokens[i];
            printf("Line: %2ld | Type: %2d | ", token.line, token.type);
            {
                int j;
                for (j=0; j<token.length; j++) {
                    printf("%c", token.start[j]);
                }
            }
            printf("\n");
        }
    }

    free(token_list.tokens);
}

int main(int argc, char** argv) {
    char* input;
    lang_state_t lang;

    if (argc < 2) {
        printf("Usage:\n\t./lang file/path.l\n");
        return 0;
    }

    input = ReadFile(argv[1]);
    Lang_Eval(input, &lang);
    free(input);

    return 0;
}

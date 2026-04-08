#define LANG_IMPLEMENTATION
#define LANG_NODE_COUNT 32
#include "lang.h"
#include <stdio.h>


int main() {
    struct lang_ctx lang = { 0 };

    puts("Lang repl");
    puts("ctrl+c to exit");

    while (1) {
        fputs("$ ", stdout);
        
        char buf[256];
        fgets(buf, sizeof(buf), stdin);
        
        struct lang_scanner scanner = {
            .line = 1,
            .start = 0,
            .current = 0,
            .input_len = sizeof(buf),
            .input = buf,
        };

        while (1) {
            struct lang_token token = Lang_Scan(&scanner, &lang);
            if (token.type == LANG_TOKEN_NONE) break;

            printf("Token: { line = %d, length = %d, type = %d, lexeme = '", token.line, token.length, token.type);
            for (unsigned int i=token.start; i<token.start+token.length; i++) {
                putc(scanner.input[i], stdout);
            }
            printf("' }\n");
        }

        puts("ok");
    }

    return 0;
}

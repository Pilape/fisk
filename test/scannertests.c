#include <stdio.h>
#include <stdlib.h>
#define __LANG_IMPLEMENTATION__
#include "../lang.h"

#include "lib/CuTest.h"

char* ReadFile(char* path) {
    FILE* fp = fopen(path, "r");

    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    rewind(fp);

    char* buffer = malloc(file_size+1);
    fread(buffer, file_size, 1, fp);
    buffer[file_size] = '\0';

    fclose(fp);

    return buffer;
}

int TokenCompareStr(_lang_token_t token, char* expected, char* file) {
    if (token.lex_length != strlen(expected)) return 0;

    for (int i=0; i<token.lex_length; i++) {
        if (file[i+token.lex_start] != expected[i]) return 0;
    }

    return 1;
}

typedef struct {
    _lang_tokentype_t type;
    size_t line;

    char* lexeme;
} test_token_t;

void TestScanner(CuTest* tc) {
    char* input = ReadFile("files/scanner_test.txt");
    _lang_tokenlist_t result = _Lang_Scan(input, strlen(input));

    test_token_t expected[] = {
        {.type = 0, .line = 1, .lexeme = "=="},
        {.type = 0, .line = 1, .lexeme = "SCANNER"},
        {.type = 0, .line = 1, .lexeme = "TEST"},
        {.type = 0, .line = 1, .lexeme = "=="},

        {.type = 0, .line = 10, .lexeme = "{"},
        {.type = 0, .line = 10, .lexeme = "square"},
        {.type = 0, .line = 10, .lexeme = "dup"},
        {.type = 0, .line = 10, .lexeme = "*"},
        {.type = 0, .line = 10, .lexeme = "}"},

        {.type = 0, .line = 13, .lexeme = "#5"},
        {.type = 0, .line = 13, .lexeme = "#25"},
        {.type = 0, .line = 13, .lexeme = "#1234567890"},

        {.type = 0, .line = 16, .lexeme = "{"},
        {.type = 0, .line = 16, .lexeme = "function"},
        {.type = 0, .line = 16, .lexeme = "}"},
        {.type = 0, .line = 16, .lexeme = "{"},
        {.type = 0, .line = 16, .lexeme = "function"},
        {.type = 0, .line = 16, .lexeme = "}"},
        {.type = 0, .line = 16, .lexeme = "{"},
        {.type = 0, .line = 16, .lexeme = "function"},
        {.type = 0, .line = 16, .lexeme = "}"},
        {.type = 0, .line = 16, .lexeme = "{"},
        {.type = 0, .line = 16, .lexeme = "function"},
        {.type = 0, .line = 16, .lexeme = "}"},

        {.type = 0, .line = 17, .lexeme = "["},
        {.type = 0, .line = 17, .lexeme = "quotation"},
        {.type = 0, .line = 17, .lexeme = "]"},
        {.type = 0, .line = 17, .lexeme = "["},
        {.type = 0, .line = 17, .lexeme = "quotation"},
        {.type = 0, .line = 17, .lexeme = "]"},
        {.type = 0, .line = 17, .lexeme = "["},
        {.type = 0, .line = 17, .lexeme = "quotation"},
        {.type = 0, .line = 17, .lexeme = "]"},
        {.type = 0, .line = 17, .lexeme = "["},
        {.type = 0, .line = 17, .lexeme = "quotation"},
        {.type = 0, .line = 17, .lexeme = "]"},

        {.type = 0, .line = 18, .lexeme = "\" string \""},
        {.type = 0, .line = 18, .lexeme = "\"string\""},
        {.type = 0, .line = 18, .lexeme = "\"string \""},
        {.type = 0, .line = 18, .lexeme = "\" string\""},
 
        {.type = 0, .line = 21, .lexeme = "#1"},
        {.type = 0, .line = 21, .lexeme = "#5"},
        {.type = 0, .line = 21, .lexeme = "\"string\""},
        {.type = 0, .line = 21, .lexeme = "+"},
        {.type = 0, .line = 21, .lexeme = "["},
        {.type = 0, .line = 21, .lexeme = "quotation"},
        {.type = 0, .line = 21, .lexeme = "]"},
        {.type = 0, .line = 21, .lexeme = "#2"},
        {.type = 0, .line = 21, .lexeme = "{"},
        {.type = 0, .line = 21, .lexeme = "function"},
        {.type = 0, .line = 21, .lexeme = "}"},
        {.type = 0, .line = 21, .lexeme = "/"},

        {.type = 0, .line = 23, .lexeme = "\"Multiline\nstring\""},

        {.type = 0, .line = 26, .lexeme = "Yippee"},

    };

    CuAssertIntEquals(tc, sizeof(expected)/sizeof(expected[0]), result.count);
    for (int i=0; i<result.count; i++) {
        CuAssertIntEquals(tc, expected[i].line, result.items[i].line);
        CuAssertIntEquals(tc, expected[i].type, result.items[i].type);

        CuAssertTrue(tc, TokenCompareStr(result.items[i], expected[i].lexeme, input));
    }

    LIST_FREE(result);
    free(input);

}

CuSuite* ScannerGetSuite() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, TestScanner);
    return suite;
}

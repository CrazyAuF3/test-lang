#include <stdio.h>
#include "lexer.h"

int main(void)
{
    const char *src =
        "let test_variable = 1;\n"
        "if (test_variable != 10.2 / 100) {\n"
        "\t.\"hello world!\";\n"
        "}\n"
        "// this is a comment line\n"
        "for (;;) { 233 }\n"
        ;
    
    Lexer lexer;
    lexer_init(&lexer, src, "program internal test");

    printf("===tokenizing internal source code===\n");
    printf("%s", src);
    printf("======\n");

    while (true) {
        Token token = scan_token(&lexer);
        print_token(token);

        if (token.type == TOKEN_EOF || token.type == TOKEN_ERROR) {
            break;
        }
    }

    if (lexer.error_msg) {
        printf("\nError: %s at line %d, col %d\n", lexer.error_msg, lexer.error_line, lexer.error_column);
    }

    return 0;
}

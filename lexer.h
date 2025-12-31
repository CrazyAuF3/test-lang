#ifndef _LEXER_H
#define _LEXER_H

#include <stdbool.h>

typedef enum {
    /* keywords */
    TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE, TOKEN_FOR, TOKEN_LET, TOKEN_GOTO,

    /* identifiers and literals */
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,

    /* operators */
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL, TOKEN_EXCLAMATION, TOKEN_EXCM_EQ,
    TOKEN_LESS, TOKEN_GREATER, TOKEN_LEQ, TOKEN_GEQ,

    /* delimiters */
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_SEMICOLON, TOKEN_COLON,

    /* special */
    TOKEN_EOF, TOKEN_ERROR,
} TokenType;

typedef struct {
    TokenType type;
    const char *start;
    int length;
    int line;
    int column;
} Token;

typedef struct {
    const char *start;
    const char *current;
    const char *source;
    int line;
    int column;
    int position;
    char *filename;

    const char *error_msg;
    int error_line;
    int error_column;

    Token peek_token;
    bool has_peek;
} Lexer;

void lexer_init(Lexer *lexer, const char *source, const char *filename);
Token scan_token(Lexer *lexer);
const char *token_name(TokenType type);
void print_token(Token token);

#endif

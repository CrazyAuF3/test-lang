#include "lexer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>  // for isdigit, isalpha, isalnum

static bool is_at_end(Lexer *lexer);
static char advance(Lexer *lexer);
static char peek(Lexer *lexer);
static char peek_next(Lexer *lexer);
static bool match(Lexer *lexer, char expected);
static void skip_whitespace(Lexer *lexer);
static Token make_token(Lexer *lexer, TokenType type);
static Token error_token(Lexer *lexer, const char *msg);
static Token scan_number(Lexer *lexer);
static Token scan_string(Lexer *lexer);
static Token scan_identifier(Lexer *lexer);

void lexer_init(Lexer *lexer, const char *source, const char *filename)
{
    lexer->source = source;
    lexer->start = source;
    lexer->current = source;
    lexer->line = 1;
    lexer->column = 1;
    lexer->position = 0;
    lexer->filename = (char*)filename;
    lexer->error_msg = NULL;
    lexer->has_peek = false;
}

Token scan_token(Lexer *lexer)
{
    skip_whitespace(lexer);
    lexer->start = lexer->current;

    if (is_at_end(lexer)) {
        return make_token(lexer, TOKEN_EOF);
    }

    char c = advance(lexer);

    if (isalpha(c)) {
        return scan_identifier(lexer);
    }
    
    if (isdigit(c)) {
        return scan_number(lexer);
    }

    #define T(_type) return make_token(lexer, _type)

    switch (c) {
        case '+': T(TOKEN_PLUS);
        case '-': T(TOKEN_MINUS);
        case '*': T(TOKEN_STAR);
        case '/': 
        if (!match(lexer, '/')) {
            T(TOKEN_SLASH);
            break;
        }
        // comments
        while (peek(lexer) != '\n' && !is_at_end(lexer)) {
            advance(lexer);
        }
        break;
        
        case '=': T(match(lexer, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '!': T(match(lexer, '=') ? TOKEN_EXCM_EQ : TOKEN_EXCLAMATION);
        case '<': T(match(lexer, '=') ? TOKEN_LEQ : TOKEN_LESS);
        case '>': T(match(lexer, '=') ? TOKEN_GEQ : TOKEN_GREATER);

        case '(': T(TOKEN_LEFT_PAREN);
        case ')': T(TOKEN_RIGHT_PAREN);
        case '{': T(TOKEN_LEFT_BRACE);
        case '}': T(TOKEN_RIGHT_BRACE);
        case ',': T(TOKEN_COMMA);
        case '.': T(TOKEN_DOT);
        case ':': T(TOKEN_COLON);
        case ';': T(TOKEN_SEMICOLON);

        case '"': return scan_string(lexer);

        default: return error_token(lexer, "unexpected char");
    }

    #undef T
}

const char *token_name(TokenType type)
{
    static const char *names[] = {
        /* keywords */
        "TOKEN_IF", "TOKEN_ELSE", "TOKEN_WHILE", "TOKEN_FOR", "TOKEN_LET", "TOKEN_GOTO",

        /* identifiers and literals */
        "TOKEN_IDENTIFIER",
        "TOKEN_NUMBER",
        "TOKEN_STRING",

        /* operators */
        "TOKEN_PLUS", "TOKEN_MINUS", "TOKEN_STAR", "TOKEN_SLASH",
        "TOKEN_EQUAL", "TOKEN_EQUAL_EQUAL", "TOKEN_EXCLAMATION", "TOKEN_EXCM_EQ",
        "TOKEN_LESS", "TOKEN_GREATER", "TOKEN_LEQ", "TOKEN_GEQ",

        /* delimiters */
        "TOKEN_LEFT_PAREN", "TOKEN_RIGHT_PAREN",
        "TOKEN_LEFT_BRACE", "TOKEN_RIGHT_BRACE",
        "TOKEN_COMMA", "TOKEN_DOT", "TOKEN_SEMICOLON", "TOKEN_COLON",

        /* special */
        "TOKEN_EOF", "TOKEN_ERROR",
    };

    return names[type];
}

void print_token(Token token)
{
    printf("[%s] Line %d, Col %d: \"%.*s\"\n",token_name(token.type), token.line, token.column, token.length, token.start);
}

static bool is_at_end(Lexer *lexer)
{
    return *(lexer->current) == '\0';
}

static char advance(Lexer *lexer)
{
    char c = *(lexer->current);
    lexer->current++;
    lexer->position++;
    lexer->column++;
    if (c == '\n') {
        lexer->column = 1;
        lexer->line++;
    }
    return c;
}

static char peek(Lexer *lexer)
{
    return *(lexer->current);
}

static char peek_next(Lexer *lexer)
{
    return *(lexer->current + 1);
}

static bool match(Lexer *lexer, char expected)
{
    if (is_at_end(lexer)) {
        return false;
    }
    if (peek(lexer) != expected) {
        return false;
    }
    advance(lexer);
    return true;
}

static void skip_whitespace(Lexer *lexer)
{
    while (true) {
        char c = peek(lexer);

        switch (c) {
            case ' ':
            case '\t':
            case '\n':
            advance(lexer);
            break;

            default:
            return;
        }
    }
}

static Token make_token(Lexer *lexer, TokenType type)
{
    Token t;
    t.type = type;
    t.start = lexer->start;
    t.line = lexer->line;
    t.column = lexer->column;
    t.length = lexer->current - lexer->start;

    return t;
}

static Token error_token(Lexer *lexer, const char *msg)
{
    Token t;
    t.type = TOKEN_ERROR;
    t.start = msg;
    t.length = strlen(msg);
    t.line = lexer->line;
    t.column = lexer->column;

    lexer->error_msg = msg;
    lexer->error_line = lexer->line;
    lexer->error_column = lexer->column;

    return t;
}

static Token scan_number(Lexer *lexer)
{
    bool has_decimal_point = false;
    while (isdigit(peek(lexer)) || peek(lexer) == '.') {
        advance(lexer);
        if (peek(lexer) == '.') {
            if (!has_decimal_point) {
                has_decimal_point = true;
            } else {
                return error_token(lexer, "invalid number expression");
            }
        }
    }

    return make_token(lexer, TOKEN_NUMBER);
}

static Token scan_string(Lexer *lexer)
{
    advance(lexer);

    char c;
    while ((c = peek(lexer)) != '"') {
        if (is_at_end(lexer)) {
            return error_token(lexer, "unterminated string");
        }
        if (c == '\\') {
            advance(lexer);

            switch (c) {
                case 'n':
                case '"':
                case '\\':
                advance(lexer);

                default:
                return error_token(lexer, "invalid escape sequence");
            }
        } else {
            advance(lexer);
        }
    }

    advance(lexer);
    return make_token(lexer, TOKEN_STRING);
}

static Token scan_identifier(Lexer *lexer)
{
    while (isalnum(peek(lexer)) || peek(lexer) == '_') {
        advance(lexer);
    }

    Token t = make_token(lexer, TOKEN_IDENTIFIER);

    const char *text = t.start;
    int length = t.length;

    #define KEYWORD(_word, _type) \
        do if (length == strlen(_word) && memcmp(text, _word, length) == 0) { t.type = _type; } while (0)
    
    KEYWORD("if", TOKEN_IF);
    KEYWORD("else", TOKEN_ELSE);
    KEYWORD("while", TOKEN_WHILE);
    KEYWORD("for", TOKEN_FOR);
    KEYWORD("let", TOKEN_LET);
    KEYWORD("goto", TOKEN_GOTO);

    #undef KEYWORD

    return t;
}

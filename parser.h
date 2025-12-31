#ifndef _PARSER_H
#define _PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct {
    Token *tokens;
    int current;
    int count;
} Parser;

// functions...

#endif

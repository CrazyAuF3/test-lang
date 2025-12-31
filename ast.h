#ifndef _AST_H
#define _AST_H

typedef enum {
    EXPR_LITERAL,
    EXPR_BINARY,
    EXPR_VARIABLE
} ExprType;

typedef struct {
    ExprType type;
    union {
        double number;
        char *string;
        struct { char *name; } variable;
        struct { ExprNode *left; char *op; ExprNode *right; } binary;
    } value;
} ExprNode;

#endif

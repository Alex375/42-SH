#ifndef EVALEXPR_H
#define EVALEXPR_H

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define operators "+-/*%^-+()"
#define whiteSpaces " \f\r\n\t\v"

enum node_type
{
    ADD = 0,
    MINUS,
    DIV,
    MULTI,
    MODULO,
    POWER,
    UNARYM,
    UNARYP,
    NUMBER
};

struct node
{
    enum node_type type;
    int value;
    struct node *left;
    struct node *right;
};

int expr_evaluate(const struct node *root);

struct node *build_r(char *line, int strt, int end);

void expr_delete(struct node *root);

int validchar(char c, char c2);

int isOperator(char c);

int myPow(int x, int y);

int eval_rpn(char *line, int len);

void my_exit(int code);

char *launch_evalexpr(char *line);

char *adapt(char *line, int len);

#endif /* !EVALEXPR_H */

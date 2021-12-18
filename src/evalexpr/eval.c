#include "evalexpr.h"

int myPow(int x, int y)
{
    int res = 1;
    for (int i = 0; i < y; i++)
    {
        res *= x;
    }
    return res;
}

int expr_evaluate(const struct node *root)
{
    if (!root)
        return 0;
    int r = expr_evaluate(root->right);
    switch (root->type)
    {
    case NUMBER:
        return root->value;
    case ADD:
    case UNARYP:
        return expr_evaluate(root->left) + expr_evaluate(root->right);
    case MINUS:
    case UNARYM:
        return expr_evaluate(root->left) - expr_evaluate(root->right);
    case MULTI:
        return expr_evaluate(root->left) * expr_evaluate(root->right);
    case DIV:
        if (r == 0)
            my_exit(2);
        return expr_evaluate(root->left) / r;
    case MODULO:
        if (r == 0)
            my_exit(2);
        return expr_evaluate(root->left) % r;
    case POWER:
        return myPow(expr_evaluate(root->left), expr_evaluate(root->right));
    }
    return 0;
}

#ifndef INC_42_SH_EVAL_AST_H
#define INC_42_SH_EVAL_AST_H

#include "execution.h"
#include "vars.h"
#include "xparser.h"

/**
** @brief               Evaluate builded ast executing commands along the way
** @param ast           the ast to evaluate.
*/
int eval_ast(struct ast *ast);

#define RETURN(val)                                                            \
    res = val;                                                                 \
    set_var_int("?", res);                                                     \
    return res;

#define EVAL_AST(val)                                                          \
    res = eval_ast(val);                                                       \
    set_var_int("?", res);                                                     \
    if (is_breaking())                                                         \
        return res;

#define EVAL_AST_IN_LOOP(val)                                                  \
    inc_depth();                                                               \
    res = eval_ast(val);                                                       \
    set_var_int("?", res);                                                     \
    if ((t = b_c_is_done()) != A_NOTHING)                                      \
    {                                                                          \
        if (t == A_BREAK)                                                      \
        {                                                                      \
            return res;                                                        \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            continued = 1;                                                     \
        }                                                                      \
    }

#endif // INC_42_SH_EVAL_AST_H

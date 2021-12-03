#ifndef INC_42_SH_EVAL_AST_H
#define INC_42_SH_EVAL_AST_H

#include "execution.h"
#include "xparser.h"
#include "vars.h"

/**
** @brief               Evaluate builded ast executing commands along the way
** @param ast           the ast to evaluate.
*/
int eval_ast(struct ast *ast, struct vars_vect *vars);

#endif // INC_42_SH_EVAL_AST_H

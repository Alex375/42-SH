#ifndef INC_42_SH_EVAL_AST_H
#define INC_42_SH_EVAL_AST_H

#include "parser.h"

/**
** @brief               Evaluate builded ast executing commands along the way
** @param ast           the ast to evaluate.
*/
int eval_ast(struct ast *ast);

#endif // INC_42_SH_EVAL_AST_H

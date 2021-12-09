#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"

struct ast *build_func(struct ast *ast, char *name)
{
    struct n_func *func = xcalloc(1, sizeof(struct n_func));
    func->ast = ast;
    func->name = name;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_FUNC;
    res->t_ast = func;

    return res;
}

struct ast *build_single(struct ast *ast, enum AST_TYPE t)
{
    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = t;
    res->t_ast = ast;

    return res;
}

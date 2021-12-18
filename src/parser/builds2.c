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

struct ast *build_case(struct tok_vect *pattern,
                       struct list_case_item *case_items)
{
    struct n_case *n_case = xcalloc(1, sizeof(struct n_case));
    n_case->pattern = pattern;
    n_case->case_items = case_items;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_CASE;
    res->t_ast = n_case;

    return res;
}

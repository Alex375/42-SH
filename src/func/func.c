#include "func.h"
#include <string.h>

#include "vars.h"
#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"
#include "handle_ast.h"

extern struct context *context;

struct fc_vect *init_func_vect()
{
    struct fc_vect *res = xcalloc(1, sizeof(struct fc_vect));

    res->cap = 8;
    res->fcs = xcalloc(res->cap, sizeof(struct fc));

    return res;
}

void add_fc(char *name, struct ast *ast)
{
    int pos = 0;
    for (; pos < context->fcs->len; ++pos)
    {
        if (!strcmp(context->fcs->fcs[pos].name, name))
            break;
    }

    if (pos != context->fcs->len)
    {
        struct ast *previous = context->fcs->fcs[pos].ast;
        context->fcs->fcs[pos].ast = handle_rec(ast, H_DUP);
        handle_rec(previous, H_FREE);

        return;
    }

    if (context->fcs->len >= context->fcs->cap - 1)
    {
        context->fcs->cap *= 2;
        context->fcs->fcs = xrecalloc(context->fcs->fcs, context->fcs->cap * sizeof(struct fc));
    }

    context->fcs->fcs[context->fcs->len].name = xstrdup(name);
    context->fcs->fcs[context->fcs->len].ast = handle_rec(ast, H_DUP);

    context->fcs->len++;
}

void destroy_fc(char *name)
{
    int pos = 0;
    for (; pos < context->fcs->len; ++pos)
    {
        if (!strcmp(context->fcs->fcs[pos].name, name))
            break;
    }

    if (pos != context->fcs->len)
    {
        struct ast *previous = context->fcs->fcs[pos].ast;
        for (int i = pos; i < context->fcs->len - 1; ++i)
        {
            context->fcs->fcs[i] = context->fcs->fcs[i + 1];
        }
        handle_rec(previous, H_FREE);

        context->fcs->len--;
    }
}

int get_fc(char *name, struct ast **ast)
{
    int pos = 0;
    for (; pos < context->fcs->len; ++pos)
    {
        if (!strcmp(context->fcs->fcs[pos].name, name))
            break;
    }

    if (pos != context->fcs->len)
        *ast = context->fcs->fcs[pos].ast;
    else
        return 0;

    return 1;
}

void free_fcs()
{
    for (int i = 0; i < context->fcs->len; ++i)
    {
        xfree(context->fcs->fcs[i].name);
        handle_rec(context->fcs->fcs[i].ast, H_FREE);
    }

    xfree(context->fcs->fcs);

    xfree(context->fcs);
}


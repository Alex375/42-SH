#include "ast_info.h"

#include "stdio.h"
#include "vars.h"
#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"

extern struct ast_info *ast_info;

void inc_depth()
{
    ast_info->depth++;
}

void set_b_c(int code, enum ast_info_type type)
{
    if (ast_info->depth != 0)
    {
        ast_info->code = code;
        ast_info->type = type;
    }
}

enum ast_info_type b_c_is_done()
{
    ast_info->depth--;

    if (ast_info->code)
        ast_info->code--;

    if (ast_info->type == A_NOTHING)
        return A_NOTHING;

    if ((ast_info->depth == 0 || ast_info->code == 0))
    {
        ast_info->code = 0;
        enum ast_info_type t = ast_info->type;
        ast_info->type = A_NOTHING;
        return t;
    }

    return A_BREAK;
}

enum ast_info_type is_breaking()
{
    return ast_info->type != A_NOTHING;
}

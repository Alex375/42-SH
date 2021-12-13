#include <ctype.h>

#include "lexer.h"

static long find_end_sub(const char *script, size_t size, enum token init_type)
{
    char start = '(';
    char end = ')';
    if (init_type == T_BACKQUOTE)
    {
        start = '`';
        end = '`';
    }

    int nb = 1;

    for (size_t i = g_lexer_info.pos; i < size; i++)
    {
        if (script[i] == end)
            nb--;
        else if (script[i] == start)
            nb++;
        if (nb == 0)
            return i;
    }

    return -1;
}

struct lexer_info new_context()
{
    struct lexer_info copy = g_lexer_info;

    g_lexer_info.fun_context = GENERAL_FUN;
    g_lexer_info.exp_context = GENERAL_EXP_HARD;
    g_lexer_info.soft_expansion = GENERAL_EXP_SOFT;
    g_lexer_info.redir_context = GENERAL_REDIR;
    g_lexer_info.var_context = GENERAL_VAR;
    g_lexer_info.for_context = GENERAL_FOR;
    g_lexer_info.last_exp_context = GENERAL_EXP_HARD;
    g_lexer_info.last_soft = GENERAL_EXP_SOFT;
    g_lexer_info.word_context = GENERAL;

    return copy;
}

void revert_context(struct lexer_info copy)
{
    g_lexer_info.fun_context = copy.fun_context;
    g_lexer_info.exp_context = copy.exp_context;
    g_lexer_info.soft_expansion = copy.soft_expansion;
    g_lexer_info.redir_context = copy.redir_context;
    g_lexer_info.var_context = copy.var_context;
    g_lexer_info.for_context = copy.for_context;
    g_lexer_info.last_exp_context = copy.last_exp_context;
    g_lexer_info.last_soft = copy.last_soft;
    g_lexer_info.word_context = copy.word_context;
}

struct token_info lex_sub(struct token_info res)
{
    enum token initial_type = res.type;
    res.type = T_COMMAND_SUB_START;
    g_lexer_info.token_list = tkvec_append(g_lexer_info.token_list, res);
    res.type = T_COMMAND_SUB_END;
    res.command = NULL;

    struct lexer_info copy = new_context();
    long end_sub = find_end_sub(g_lexer_info.script, g_lexer_info.script_size,
                                initial_type);

    if (end_sub == -1)
    {
        res.type = T_ERROR;
        res.command = NULL;
        return res;
    }

    lexer_start(g_lexer_info.script, g_lexer_info.script_size, end_sub);

    revert_context(copy);
    g_lexer_info.pos++;
    if (g_lexer_info.script[g_lexer_info.pos] == '"')
    {
        g_lexer_info.last_soft = IN_DQUOTE;
        g_lexer_info.soft_expansion = (g_lexer_info.soft_expansion == IN_DQUOTE)
            ? GENERAL_EXP_SOFT
            : IN_DQUOTE;
        g_lexer_info.pos++;
    }

    res.is_space_after = isblank(g_lexer_info.script[g_lexer_info.pos])
        || g_lexer_info.script[g_lexer_info.pos] == '\n';

    return res;
}

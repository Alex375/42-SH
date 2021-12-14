#include "lexer.h"

void lexer_start(char *script, size_t size, long end)
{
    if (end == -1)
    {
        if (g_lexer_info.token_list != NULL)
            return;

        g_lexer_info.script = script;
        g_lexer_info.script_size = size;
        g_lexer_info.token_list = tkvec_create();
    }

    struct token_info token;
    while ((token = tokenify_next(script, size)).type != T_EOF)
    {
        g_lexer_info.token_list = tkvec_append(g_lexer_info.token_list, token);
        if (end != -1 && g_lexer_info.pos >= (size_t)end)
        {
            return;
        }
    }
}

void lexer_reset()
{
    g_lexer_info.fun_context = GENERAL_FUN;
    g_lexer_info.exp_context = GENERAL_EXP_HARD;
    g_lexer_info.soft_expansion = GENERAL_EXP_SOFT;
    g_lexer_info.redir_context = GENERAL_REDIR;
    g_lexer_info.var_context = GENERAL_VAR;
    g_lexer_info.for_context = GENERAL_FOR;
    g_lexer_info.last_exp_context = GENERAL_EXP_HARD;
    g_lexer_info.last_soft = GENERAL_EXP_SOFT;
    g_lexer_info.word_context = GENERAL;
    g_lexer_info.array_pos = 0;
    g_lexer_info.pos = 0;
    g_lexer_info.script = NULL;
    g_lexer_info.script_size = 0;
    if (g_lexer_info.token_list != NULL)
        tkvec_free(g_lexer_info.token_list);
    g_lexer_info.token_list = NULL;
}

struct lexer_info save_lexer()
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
    g_lexer_info.array_pos = 0;
    g_lexer_info.pos = 0;
    g_lexer_info.script = NULL;
    g_lexer_info.script_size = 0;
    g_lexer_info.token_list = NULL;

    return copy;
}

void revert_lexer(struct lexer_info copy)
{
    g_lexer_info.case_context = copy.case_context;
    g_lexer_info.fun_context = copy.fun_context;
    g_lexer_info.exp_context = copy.exp_context;
    g_lexer_info.soft_expansion = copy.soft_expansion;
    g_lexer_info.redir_context = copy.redir_context;
    g_lexer_info.var_context = copy.var_context;
    g_lexer_info.for_context = copy.for_context;
    g_lexer_info.last_exp_context = copy.last_exp_context;
    g_lexer_info.last_soft = copy.last_soft;
    g_lexer_info.word_context = copy.word_context;
    g_lexer_info.token_list = copy.token_list;
    g_lexer_info.array_pos = copy.array_pos;
    g_lexer_info.pos = copy.pos;
    g_lexer_info.script = copy.script;
    g_lexer_info.script_size = copy.script_size;
}


#include "lexer.h"

int skip_character(char c)
{
    if (g_lexer_info.eval_context == IN_EVAL)
        return 0;
    if (c == '\n' && g_lexer_info.exp_context == IN_ESCAPE_EXP)
    {
        return 1;
    }
    if (c == '\\' && g_lexer_info.exp_context == GENERAL_EXP_HARD
        && g_lexer_info.soft_expansion == GENERAL_EXP_SOFT)
    {
        return 1;
    }
    if (c == '\'' && g_lexer_info.exp_context != IN_ESCAPE_EXP
        && g_lexer_info.soft_expansion != IN_DQUOTE)
    {
        return 1;
    }
    if (c == '"' && g_lexer_info.exp_context == GENERAL_EXP_HARD)
    {
        return 1;
    }
    if (c == '`')
    {
        return 0;
    }
    if ((c == '{' || c == '}') && g_lexer_info.var_context == IN_VAR)
    {
        return 1;
    }

    return 0;
}

int detect_context(char c)
{
    if (g_lexer_info.exp_context == GENERAL_EXP_HARD
        && g_lexer_info.last_exp_context == IN_ESCAPE_EXP)
    {
        g_lexer_info.last_exp_context = IN_SQUOTE_EXP;
    }
    if (c == '\\' && g_lexer_info.exp_context == GENERAL_EXP_HARD)
    {
        g_lexer_info.exp_context = IN_ESCAPE_EXP;
        g_lexer_info.last_exp_context = IN_ESCAPE_EXP;
    }
    else if (c == '\'' && g_lexer_info.exp_context != IN_ESCAPE_EXP
             && g_lexer_info.soft_expansion != IN_DQUOTE)
    {
        g_lexer_info.last_exp_context = IN_SQUOTE_EXP;
        g_lexer_info.exp_context = (g_lexer_info.exp_context == IN_SQUOTE_EXP)
            ? GENERAL_EXP_HARD
            : IN_SQUOTE_EXP;
    }
    else if (c == '$' && g_lexer_info.for_context != VAR_FOR
             && g_lexer_info.exp_context == GENERAL_EXP_HARD)
    {
        if (g_lexer_info.pos + 1 >= g_lexer_info.script_size
            || g_lexer_info.script[g_lexer_info.pos + 1] != '(')
            g_lexer_info.var_context = IN_VAR;
    }
    else if (c == '"' && g_lexer_info.exp_context == GENERAL_EXP_HARD)
    {
        g_lexer_info.last_soft = IN_DQUOTE;
        g_lexer_info.soft_expansion = (g_lexer_info.soft_expansion == IN_DQUOTE)
            ? GENERAL_EXP_SOFT
            : IN_DQUOTE;
    }
    else if (g_lexer_info.exp_context == IN_ESCAPE_EXP)
    {
        g_lexer_info.exp_context = GENERAL_EXP_HARD;
    }
    else if (c == '}' && g_lexer_info.fun_context == IN_FUN)
    {
        g_lexer_info.word_context = GENERAL;
        return 1;
    }
    return 0;
}

void context_update(struct token_info res)
{
    if (is_token_seperator(res.type))
    {
        g_lexer_info.word_context = GENERAL;
        g_lexer_info.var_context = GENERAL_VAR;
    }
}

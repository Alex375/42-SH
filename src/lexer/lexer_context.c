#include "lexer.h"

int detect_context(char c)
{
    if (c == '\'')
    {
        g_lexer_info.last_exp_context = IN_SQUOTE_EXP;
        if (g_lexer_info.exp_context == IN_SQUOTE_EXP)
        {
            g_lexer_info.exp_context = GENERAL_EXP;
        }
        else
        {
            g_lexer_info.exp_context = IN_SQUOTE_EXP;
        }
        return 1;
    }
    else if (c == '$' && g_lexer_info.for_context != VAR_FOR && g_lexer_info.exp_context == GENERAL_EXP)
    {
        g_lexer_info.var_context = IN_VAR;
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

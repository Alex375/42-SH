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

    return 0;
}

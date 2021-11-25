#include "lexer.h"

int detect_context(char c)
{
    if (c == '\'')
    {
        g_lexer_info.last_context = IN_SQUOTE;
        if (g_lexer_info.exp_context == IN_SQUOTE)
        {
            g_lexer_info.exp_context = GENERAL;
        }
        else
        {
            g_lexer_info.exp_context = IN_SQUOTE;
        }
        return 1;
    }

    return 0;
}

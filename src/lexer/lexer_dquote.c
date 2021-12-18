#include <ctype.h>

#include "lexer.h"

int look_ahead_dquote(const char *script, size_t size, size_t acu_size)
{
    size_t i = g_lexer_info.pos;

    if (script[i] == '$' && i > 0 && script[i - 1] == '$')
    {
        return 1;
    }
    if (script[i] == '$' && acu_size > 0)
    {
        return 0;
    }
    if (script[i] == '"' && g_lexer_info.var_context == IN_VAR)
    {
        g_lexer_info.pos++;
        g_lexer_info.soft_expansion = GENERAL_EXP_SOFT;
        return 0;
    }

    if (i > size)
        return 0;

    return 1;
}

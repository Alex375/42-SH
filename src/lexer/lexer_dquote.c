#include "lexer.h"
#include <ctype.h>

int look_ahead_dquote(const char* script, size_t size)
{
    size_t i = g_lexer_info.pos;

    if (script[i] == ' ' && g_lexer_info.var_context == IN_VAR)
    {
        return 0;
    }

    if (i > size)
        return 0;

    return 1;
}

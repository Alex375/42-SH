#include "lexer.h"
#include <ctype.h>

int look_ahead_dquote(const char* script, size_t size)
{
    size_t i = g_lexer_info.pos;

    if (script[i] == '$' && i > 0 && script[i - 1] == '$')
    {
        return 1;
    }
    else if (script[i] == '$' && i > 0 && script[i - 1] != '"')
    {
        return 0;
    }


    if (i > size)
        return 0;

    return 1;
}

#include "lexer.h"

int look_ahead_squote(size_t size)
{
    size_t i = g_lexer_info.pos;

    if (i > size)
        return 0;

    return 1;
}

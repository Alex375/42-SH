#include "lexer.h"

int look_ahead_squote(const char *script, size_t size)
{
    size_t i = g_lexer_info.pos;

    if (i > size)
        return 0;

    if (script[i] == '\'')
    {
        return 0;
    }

    return 1;
}

struct token_info lex_squote(struct token_info res, struct string *string)
{
    if (g_lexer_info.exp_context == IN_SQUOTE_EXP)
        g_lexer_info.exp_context = GENERAL_EXP;
    else
        g_lexer_info.exp_context = IN_SQUOTE_EXP;
    string_free(string);

    return res;
}

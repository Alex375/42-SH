#include <lexer.h>

struct token_info lex_command(struct token_info res, struct string *string)
{
    if (g_lexer_info.exp_context == IN_SQUOTE)
    {
        res.type = T_ERROR;
        res.command = NULL;
        string_free(string);
    }
    else
    {
        res.type = T_WORD;
        res.command = string_get(string);
    }
    return res;
}

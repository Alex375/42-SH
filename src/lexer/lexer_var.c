#include "lexer.h"
#include <ctype.h>


struct token_info lex_var(struct token_info res, struct string *string)
{
    if (string->size == 1 && string->data[0] == '$')
    {
        res.type = T_WORD;
    }
    else if (string->size != 0)
    {
        res.type = T_VAR;
    }
    res.command = string_get(string);
    if (string->data[0] == '$' && string->size > 1)
    {
        res.command++;
    }
    g_lexer_info.var_context = GENERAL_VAR;

    return res;
}

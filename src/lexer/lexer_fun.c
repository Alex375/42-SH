#include "lexer.h"

struct token_info lex_fun(struct token_info res, struct string *string)
{
    if (string->size < 3)
    {
        res.type = T_ERROR;
    }
    else
    {
        string->data[string->size - 2] = '\0';
        res.command = string_get(string);
        res.type = T_FUN_INIT;
        g_lexer_info.fun_context = IN_FUN;
    }

    return res;
}

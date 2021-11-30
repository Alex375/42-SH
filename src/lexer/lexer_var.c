#include "lexer.h"
#include <ctype.h>

int is_valid_var(const char *string)
{
    return fnmatch("$?(+([a-zA-Z0-9_])|[@*?$#])", string, FNM_EXTMATCH) == 0;
}

struct token_info lex_varname(struct token_info res, struct string *string)
{
    res.type = T_VAR_INIT;

    g_lexer_info.var_context = IN_VAR_VALUE;
    string->data[--string->size] = '\0';

    res.command = string_get(string);
    return res;
}

struct token_info lex_varvalue(struct token_info res, struct string *string)
{
    res.type = T_VAR_VALUE;

    g_lexer_info.var_context = GENERAL_VAR;

    res.command = string_get(string);
    return res;
}

struct token_info lex_var(struct token_info res, struct string *string)
{
    if (string->size == 1 && string->data[0] == '$')
    {
        res.type = T_WORD;
    }
    else if (string->size != 0)
    {
        res.type = T_VAR;
        if (g_lexer_info.last_soft == IN_DQUOTE)
            res.type = T_VAR_INQUOTE;
    }
    size_t size = string->size;
    res.command = string_get(string);

    if (size > 1 && res.command[0] == '$')
    {
        res.command++;
    }
    g_lexer_info.var_context = GENERAL_VAR;

    return res;
}

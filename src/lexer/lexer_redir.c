#include <ctype.h>
#include <string.h>

#include "lexer.h"

static int is_number(const char *str)
{
    if (str == NULL)
        return 0;
    int i = 0;

    while (str[i])
    {
        if (!isdigit(str[i]))
            return 0;
        i++;
    }

    return 1;
}

int is_ionumber(struct token_info res, struct string *string)
{
    return res.type == T_WORD && is_number(string->data) && string->size == 1
        && g_lexer_info.pos < g_lexer_info.script_size
        && (g_lexer_info.script[g_lexer_info.pos] == '>'
            || g_lexer_info.script[g_lexer_info.pos] == '<');
}

struct token_info lex_ionumber(struct token_info res, struct string *string)
{
    if (g_lexer_info.exp_context == IN_SQUOTE_EXP)
    {
        res.type = T_ERROR;
        res.command = NULL;
        string_free(string);
    }
    else
    {
        res.type = T_IONUMBER;
        res.command = string_get(string);
    }
    return res;
}

#include <lexer.h>

struct token_info lex_command(struct token_info res, struct string *string)
{
    if (g_lexer_info.exp_context == IN_SQUOTE_EXP)
    {
        res.type = T_ERROR;
        res.command = NULL;
        string_free(string);
    }
    else
    {
        g_lexer_info.word_context = IN_COMMAND;
        res.type = T_WORD;
        res.command = string_get(string);
    }
    return res;
}

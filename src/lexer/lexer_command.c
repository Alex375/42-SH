#include <lexer.h>

struct token_info lex_command(struct token_info res, struct string *string)
{
    if (g_lexer_info.context == IN_COMMAND)
    {
        res.type = T_COMMAND_ARG;
    }

    g_lexer_info.context = IN_COMMAND;
    res.command = string_get(string);

    return res;
}

#include <lexer.h>

struct token_info lex_command(struct token_info res, struct string *string)
{
    res.type = T_WORD;
    res.command = string_get(string);
    return res;
}

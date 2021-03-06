#include "lexer.h"

int look_ahead_keywords(const char *script, size_t size)
{
    size_t i = g_lexer_info.pos;

    if (i > size)
        return 1;

    struct string *accumulator = string_create();

    if (script[i] == '$' && i > 0 && script[i - 1] == '$')
    {
        string_free(accumulator);
        return 1;
    }
    if (script[i] == '$' && i > 0)
    {
        string_free(accumulator);
        return 0;
    }

    if (script[i] == '}' && g_lexer_info.fun_context == IN_FUN)
    {
        string_free(accumulator);
        return 1;
    }

    if (g_lexer_info.var_context == IN_VAR && i > 0 && script[i - 1] != '$'
        && (script[i] == '"' || script[i] == '\''))
        return 0;

    accumulator = string_append(accumulator, script[i]);
    if (separatorify(accumulator->data) != -1)
    {
        if (g_lexer_info.var_context == GENERAL_VAR
            || g_lexer_info.var_context == IN_VAR_INIT)
        {
            if ((script[i] == '(' && i + 1 < size && script[i + 1] == ')')
                || (script[i] == ')' && i > 0 && script[i - 1] == '('))
            {
                return 1;
            }
            else
            {
                string_free(accumulator);
                return 0;
            }
        }
    }

    i++;
    if (i > size)
        return 0;

    accumulator = string_append(accumulator, script[i]);
    if (separatorify(accumulator->data) != -1)
    {
        if (g_lexer_info.var_context == GENERAL_VAR
            || g_lexer_info.var_context == IN_VAR_INIT)
        {
            string_free(accumulator);
            return 0;
        }
    }

    string_free(accumulator);
    return 1;
}

struct token_info lex_keywords(struct token_info res, struct string *string)
{
    if (res.type == T_FOR)
    {
        g_lexer_info.for_context = VAR_FOR;
    }
    else if (res.type >= T_REDIR_1 && res.type <= T_REDIR_PIPE)
    {
        g_lexer_info.redir_context = IN_REDIR;
    }
    else if (res.type == T_C_BRKT)
    {
        g_lexer_info.fun_context = GENERAL_FUN;
    }
    else if (g_lexer_info.case_context == IN_CASE)
    {
        if (res.type == T_C_PRTH)
        {
            g_lexer_info.word_context = GENERAL;
            g_lexer_info.case_context = IN_CASE_COMMAND;
        }
        else if (res.type == T_CASE || res.type == T_IN
                 || is_token_seperator(res.type))
        {
            g_lexer_info.word_context = IN_COMMAND;
        }
    }
    else if (res.type == T_CASE)
    {
        g_lexer_info.case_context = IN_CASE;
        g_lexer_info.word_context = IN_COMMAND;
    }
    if (res.type == T_SEMICOLON && g_lexer_info.token_list->size > 0
        && g_lexer_info.token_list->data[g_lexer_info.token_list->size - 1].type
            == T_SEMICOLON
        && g_lexer_info.case_context == IN_CASE_COMMAND)
    {
        g_lexer_info.case_context = IN_CASE;
    }

    if (string != NULL)
        string_free(string);

    return res;
}


#include <string.h>

#include "lexer.h"

struct words_converter converter = {
    35,
    21,
    { "if", "then", "elif", "else", "fi",   "while", "until", "for", "in",
      "do", "done", "!",    "||",   "&&",   "\n",    ";",     "{",   "}",
      "(",  ")",    "|",    ">",    "<",    ">&",    "<&",    ">>",  "<>",
      ">|", "`",    "$(",   "case", "esac", ";;",    "$((",   "))" },
    { "||", "&&", "\n", ";",  "(",  ")", "|",  " ",  "\0",  ">", "<",
      ">&", "<&", ">>", "<>", ">|", "`", "$(", ";;", "$((", "))" }
};

int separatorify(const char *token_str)
{
    if (token_str == NULL || token_str[0] == '\0')
        return -1;
    size_t i = 0;

    /* CHECK IF WE CAN CONVERT INTO A KNOWN SEPARATOR */
    while (i < converter.nb_separator
           && strcmp(token_str, converter.separator[i]) != 0)
        i++;

    /* UNKOWN SEPARATOR */
    if (i >= converter.nb_separator)
        return -1;

    /* KNOWN SEPARATOR */
    return i;
}

int is_token_seperator(enum token token)
{
    enum token sep[] = {
        T_AND,        T_OR,        T_NEWLINE, T_SEMICOLON,     T_C_PRTH,
        T_O_PRTH,     T_PIPE,      T_EOF,     T_REDIR_1,       T_REDIR_2,
        T_REDIR_O_2,  T_REDIR_O_2, T_REDIR_A, T_REDIR_I_1,     T_REDIR_I_A,
        T_REDIR_PIPE, T_BACKQUOTE, T_D_PAREN, T_DOUBLE_SCOLON, T_EVALEXPR
    };
    size_t nb_sep = sizeof(sep) / sizeof(enum token);

    for (size_t i = 0; i < nb_sep; ++i)
    {
        if (sep[i] == token)
            return 1;
    }
    return 0;
}

enum token tokenify(const char *token_str)
{
    if (token_str == NULL || token_str[0] == '\0')
        return -1;
    size_t i = 0;

    /* CHECK IF WE CAN CONVERT INTO A KNOWN TOKEN */
    while (i < converter.nb_token
           && strcmp(converter.token_converter[i], token_str) != 0)
        i++;

    /* UNKOWN TOKEN */
    if (i >= converter.nb_token)
        return T_WORD;

    /* KNOWN TOKEN */
    return i;
}

int check_special(struct string *accumulator, char next_char)
{
    if (g_lexer_info.eval_context == IN_EVAL)
    {
        if (strcmp(accumulator->data, "))") == 0)
            return 1;
        return 0;
    }

    if (g_lexer_info.exp_context == GENERAL_EXP_HARD && g_lexer_info.pos > 0
        && g_lexer_info.pos + 1 < g_lexer_info.script_size)
    {
        size_t i = g_lexer_info.pos;
        struct string *temp = string_create();
        temp = string_append(temp, g_lexer_info.script[i - 1]);
        temp = string_append(temp, g_lexer_info.script[i]);
        temp = string_append(temp, g_lexer_info.script[i + 1]);
        int tmp = separatorify(temp->data);
        string_free(temp);
        if (tmp == 19)
        {
            g_lexer_info.pos += 2;
            g_lexer_info.eval_context = IN_EVAL;
            return 2;
        }
    }

    if (g_lexer_info.var_context == IN_VAR && !is_valid_var(accumulator->data))
    {
        accumulator->size--;
        accumulator->data[accumulator->size] = '\0';
        g_lexer_info.pos--;
        return 1;
    }

    if (g_lexer_info.soft_expansion == IN_DQUOTE
        && g_lexer_info.exp_context == IN_ESCAPE_EXP)
    {
        char next[2] = { next_char, 0 };
        if (fnmatch("[$\\\\\"`]", next, FNM_EXTMATCH) == 0
            || fnmatch("$(", next, FNM_EXTMATCH) == 0)
        {
            accumulator->size--;
            accumulator->data[accumulator->size] = '\0';
        }
    }

    if (g_lexer_info.soft_expansion == IN_DQUOTE)
    {
        if (fnmatch("$(", accumulator->data, FNM_EXTMATCH) == 0
            || fnmatch("`", accumulator->data, FNM_EXTMATCH) == 0)
        {
            return 1;
        }
        else if (fnmatch("$(", accumulator->data, FNM_EXTMATCH) == 0)
        {
            return 1;
        }
    }

    if (g_lexer_info.soft_expansion == GENERAL_EXP_SOFT
        && g_lexer_info.exp_context == GENERAL_EXP_HARD
        && g_lexer_info.word_context == GENERAL)
    {
        if (fnmatch("+([a-zA-Z0-9_])=", accumulator->data, FNM_EXTMATCH) == 0)
        {
            g_lexer_info.var_context = IN_VAR_NAME;
            return 1;
        }
    }

    if (g_lexer_info.last_exp_context == IN_ESCAPE_EXP)
    {
        if (fnmatch("$", accumulator->data, FNM_EXTMATCH) == 0)
        {
            g_lexer_info.exp_context = GENERAL_EXP_HARD;
            return 1;
        }
    }

    if (g_lexer_info.exp_context != GENERAL_EXP_HARD
        || g_lexer_info.last_exp_context != GENERAL_EXP_HARD
        || g_lexer_info.soft_expansion != GENERAL_EXP_SOFT)
    {
        return 0;
    }

    int token;

    if (g_lexer_info.pos < g_lexer_info.script_size && accumulator->size >= 1)
    {
        accumulator = string_append(accumulator, next_char);
        token = separatorify(accumulator->data);
        if ((token != -1 && token != 20)
            || (token == 20 && g_lexer_info.eval_context == IN_EVAL))
        {
            g_lexer_info.pos++;
            return 1;
        }
        accumulator = string_pop(accumulator, NULL);
    }

    if (g_lexer_info.var_context == GENERAL_VAR
        && g_lexer_info.for_context == GENERAL_FOR)
    {
        if (fnmatch("*@([(])@([)])", accumulator->data, FNM_EXTMATCH) == 0)
        {
            g_lexer_info.fun_context = IN_FUN_NAME;
            return 1;
        }
    }

    token = separatorify(accumulator->data);
    if (token == -1)
        return 0;

    return 1;
}

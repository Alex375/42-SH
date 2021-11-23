#ifndef INC_42_SH_TYPE_H
#define INC_42_SH_TYPE_H

#include <stddef.h>

/**
** @brief                   Describe a keyword in shell.
*/
enum token
{
    T_IF,
    T_THEN,
    T_ELSE,
    T_FI,
    T_NOT,
    T_OR,
    T_AND,
    T_NEWLINE,
    T_SEMICOLON,
    T_QUOTE,
    T_O_BRKT,
    T_C_BRKT,
    T_O_PRTH,
    T_C_PRTH,
    T_PIPE,
    T_EOF,
    T_COMMAND
};

/**
** @brief                   Linked-list of every token in a block.
** @param type              Type of the element.
** @param command           Command string.
** @param next              Next element of the list.
*/
struct token_info
{
    enum token type;
    char *command;
};

/**
** @brief                   Global info for the lexer
** @param nb_token          The number of token in the token converter.
** @param nb_separator      The number of separator in the separator list.
** @param pos               Index pos in the script string.
** @param token_convertor   Array for converting string into token.
** @param separator         Array of useful seperator for the lexer.
*/
struct lexer_info
{
    size_t nb_token;
    size_t nb_separator;
    size_t pos;
    char *token_converter[16];
    char separator[3];
};

/**
** @brief                   Token-ify the next element of a script.
** @param script            Script to be lex.
** @param size              Size in character of the script.
*/
struct token_info get_next_token(const char *script, size_t size);


#endif // INC_42_SH_TYPE_H

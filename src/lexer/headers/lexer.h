#ifndef INC_42_SH_TYPE_H
#define INC_42_SH_TYPE_H

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
    T_COMMAND,
    T_NEWLINE,
    T_SEMICOLON,
    T_EOF,
    T_QUOTE,
    T_O_BRKT,
    T_C_BRKT,
    T_O_PRTH,
    T_C_PRTH
};

/**
** @brief                   Linked-list of every token in a block.
** @param type              Type of the element.
** @param command           Command string.
** @param next              Next element of the list.
*/
struct lexer_list
{
    enum token type;
    char *command;
    struct lexer_list *next;
};

#endif // INC_42_SH_TYPE_H

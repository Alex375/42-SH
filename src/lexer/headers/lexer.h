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
    T_QUOTE
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

#ifndef INC_42_SH_TOKENS_H
#define INC_42_SH_TOKENS_H

/**
** @brief                   Describe a keyword in shell.
*/
enum token
{
    T_IF,
    T_THEN,
    T_ELIF,
    T_ELSE,
    T_FI,
    T_WHILE,
    T_UNTIL,
    T_FOR,
    T_IN,
    T_DO,
    T_DONE,
    T_NOT,
    T_OR,
    T_AND,
    T_NEWLINE,
    T_SEMICOLON,
    T_O_BRKT,
    T_C_BRKT,
    T_O_PRTH,
    T_C_PRTH,
    T_PIPE,
    T_REDIR_1,
    T_REDIR_I_1,
    T_REDIR_A,
    T_REDIR_I_A,
    T_REDIR_2,
    T_REDIR_O_2,
    T_REDIR_PIPE,
    T_BACKQUOTE,
    T_D_PAREN,
    T_CASE,
    T_ESAC,
    T_DOUBLE_SCOLON,
    T_EOF,
    T_IONUMBER,
    T_VAR,
    T_VAR_INQUOTE,
    T_VAR_INIT,
    T_FUN_INIT,
    T_COMMAND_SUB_START,
    T_COMMAND_SUB_END,
    T_WORD,
    T_ERROR
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
    char is_space_after;
};

#endif // INC_42_SH_TOKENS_H

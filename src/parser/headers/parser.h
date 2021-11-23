#ifndef INC_42_SH_PARSER_H
#define INC_42_SH_PARSER_H

/**
** @brief                   Describe the node type in ast.
*/
typedef enum AST_TYPE
{
    AST_CMD,
    AST_IF,
    AST_WHILE,
    AST_FOR,
    AST_CASE,
    AST_UNTIL,
    AST_PIPE,
    AST_REDIR,
    AST_FUNC,
    AST_NOT,
    AST_AND,
    AST_OR,
    AST_BRACKET,
    AST_PARENTH
} AST_TYPE;

/**
** @brief                   A flexible ast member.
*/
typedef struct ast
{
    AST_TYPE type;
    void *t_ast;
} ast;

/**
** @brief                   <<AST_CMD>> ast member.
*/
typedef struct n_cmd
{
    char *cmd_line;
} n_cmd;


/**
** @brief                   <<AST_IF>> ast member.
*/
typedef struct n_if
{
    ast *condition;
    ast *true;
    ast *false;
} n_if;

/**
** @brief                   <<AST_NOT>> ast member.
*/
typedef struct n_not
{
    ast *ast;
} n_not;

/**
** @brief                   <<AST_AND, AST_OR>> ast member.
*/
typedef struct n_binary
{
    ast *left;
    ast *right;
} n_binary;

#endif // INC_42_SH_PARSER_H

#include <errno.h>
#include <stddef.h>
#include <stdio.h>

#include "parser.h"
#include "string.h"

void tab(int prof)
{
    for (int i = 0; i < prof; ++i)
    {
        printf("\t");
    }
}

void pp_rec(struct ast *ast, int prof)
{
    if (!ast)
    {
        tab(prof);
        printf("<< NULL >>\n");
        return;
    }

    struct n_s_cmd *s_cmd_ast;
    struct n_binary *binary_ast;
    struct n_if *if_ast;
    struct n_command *cmd_ast;

    switch (ast->type)
    {
    case AST_S_CMD:
        s_cmd_ast = ast->t_ast;
        tab(prof);
        printf("COMMAND = %s : %s\n", s_cmd_ast->cmd, s_cmd_ast->cmd_arg->data);
        break;
    case AST_IF:
        if_ast = ast->t_ast;
        tab(prof);
        printf("IF------\n");
        pp_rec(if_ast->condition, prof + 1);
        tab(prof);
        printf("THEN------\n");
        pp_rec(if_ast->true, prof + 1);
        if (if_ast->false)
        {
            tab(prof);
            printf("ELSE------\n");
            pp_rec(if_ast->false, prof + 1);
        }
        break;
    case AST_WHILE:
        break;
    case AST_FOR:
        break;
    case AST_CASE:
        break;
    case AST_UNTIL:
        break;
    case AST_PIPE:
        binary_ast = ast->t_ast;
        pp_rec(binary_ast->left, prof + 1);
        tab(prof);
        printf("<< | >>\n");
        pp_rec(binary_ast->right, prof + 1);

        break;
    case AST_REDIR:
        break;
    case AST_FUNC:
        break;
    case AST_LIST:
        binary_ast = ast->t_ast;
        tab(prof);
        printf("<< LIST : >>\n");
        pp_rec(binary_ast->left, prof + 1);
        tab(prof);
        printf("<< --- >>\n");
        pp_rec(binary_ast->right, prof + 1);
        break;
    case AST_NOT:
        tab(prof);
        printf("<< ! >>\n");
        pp_rec(ast->t_ast, prof + 1);
        break;
    case AST_AND:
        binary_ast = ast->t_ast;
        pp_rec(binary_ast->left, prof + 1);
        tab(prof);
        printf("<< && >>\n");
        pp_rec(binary_ast->right, prof + 1);

        break;
    case AST_OR:
        binary_ast = ast->t_ast;
        pp_rec(binary_ast->left, prof + 1);
        tab(prof);
        printf("<< || >>\n");
        pp_rec(binary_ast->right, prof + 1);

        break;
    case AST_BRACKET:
        break;
    case AST_PARENTH:
        break;
    case AST_CMD:
        cmd_ast = ast->t_ast;
        pp_rec(cmd_ast->ast, prof + 1);
        tab(prof);
        printf("REDIRS : ");
        struct list_redir *tmp = cmd_ast->redirs;
        while (tmp)
        {
            printf("%s into %s |", tmp->ionumber, tmp->word);
            tmp = tmp->next;
        }
        printf("\n");
        break;
    }
}

void ast_pretty_print(char *script, size_t size)
{
    struct ast *ast;
    errno = 0;

    // struct token_info t;
    // while ((t = pop_token(script, size)).type != T_EOF);

    lexer_start(script, size);
    while (errno != ERROR_EMPTY_EOF)
    {
        errno = 0;
        ast = parse_input();
        if (errno != 0)
        {
            if (errno == ERROR_PARSING)
                printf("!!! ERROR WHILE PARSING \n");
            break;
        }
        else
            pp_rec(ast, 0);
    }
    lexer_reset();
}

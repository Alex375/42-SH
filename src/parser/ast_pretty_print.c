#include <errno.h>
#include <stdio.h>

#include "xparser.h"

void tab(int prof)
{
    for (int i = 0; i < prof; ++i)
    {
        printf("\t");
    }
}

void print_redir(struct list_redir *redirs, int prof)
{
    if (redirs)
    {
        tab(prof);
        printf("REDIRS : ");
        while (redirs)
        {
            printf("%s into ... | ", redirs->ionumber);
            redirs = redirs->next;
        }
        printf("\n");
    }
}

void print_assign(struct list_var_assign *vars, int prof)
{
    if (vars)
    {
        tab(prof);
        printf("ASSIGNEMENT : ");
        while (vars)
        {
            printf("%s = ... | ", vars->name);
            vars = vars->next;
        }
        printf("\n");
    }
}

void pp_rec(struct ast *ast, int prof);

void pp_rec3(struct ast *ast, int prof)
{
    struct n_binary *binary_ast;
    struct n_command *cmd_ast;

    switch (ast->type)
    {
    case AST_LIST:
        binary_ast = ast->t_ast;
        tab(prof);
        printf("LIST : \n");
        pp_rec(binary_ast->left, prof + 1);
        tab(prof);
        printf("----   \n");
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
    case AST_CMD:
        cmd_ast = ast->t_ast;
        pp_rec(cmd_ast->ast, prof);
        print_redir(cmd_ast->redirs, prof);
        break;
    default:
        return;
    }
}

void pp_rec2(struct ast *ast, int prof)
{
    struct n_binary *binary_ast;
    struct n_for *for_ast;
    struct n_func *func;

    switch (ast->type)
    {
    case AST_FOR:
        for_ast = ast->t_ast;
        tab(prof);
        printf("FOR------\n");
        tab(prof + 1);
        printf("%s ", for_ast->name);

        printf("\n");
        tab(prof);
        printf("DO------\n");
        pp_rec(for_ast->statement, prof + 1);
        tab(prof);
        printf("DONE------\n");
        break;
    case AST_CASE:
        break;
    case AST_PIPE:
        binary_ast = ast->t_ast;
        pp_rec(binary_ast->left, prof + 1);
        tab(prof);
        printf("<< | >>\n");
        pp_rec(binary_ast->right, prof + 1);

        break;
    case AST_SUBSHELL:
        tab(prof);
        printf("SUBSHELL\n");
        pp_rec(ast->t_ast, prof + 1);
        break;
    case AST_FUNC:
        func = ast->t_ast;
        tab(prof);
        printf("FUNCDEC %s\n", func->name);
        pp_rec(func->ast, prof + 1);
        break;
    default:
        pp_rec3(ast, prof);
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

    switch (ast->type)
    {
    case AST_S_CMD:
        s_cmd_ast = ast->t_ast;
        tab(prof);
        printf("COMMAND\n");
        print_assign(s_cmd_ast->vars, prof);
        break;
    case AST_IF:
        if_ast = ast->t_ast;
        tab(prof);
        printf("IF------\n");
        pp_rec(if_ast->condition, prof + 1);
        tab(prof);
        printf("THEN------\n");
        pp_rec(if_ast->true_c, prof + 1);
        if (if_ast->false_c)
        {
            tab(prof);
            printf("ELSE------\n");
            pp_rec(if_ast->false_c, prof + 1);
        }
        tab(prof);
        printf("FI------\n");
        break;
    case AST_WHILE:
    case AST_UNTIL:
        binary_ast = ast->t_ast;
        tab(prof);
        if (ast->type == AST_WHILE)
            printf("WHILE------\n");
        else
            printf("UNTIL------\n");
        pp_rec(binary_ast->left, prof + 1);
        tab(prof);
        printf("DO------\n");
        pp_rec(binary_ast->right, prof + 1);
        tab(prof);
        printf("DONE------\n");
        break;
    default:
        pp_rec2(ast, prof);
    }
}

void ast_pretty_print(struct ast *ast)
{
    if (!ast)
        return;
    if (errno == ERROR_PARSING)
        printf("!!! ERROR WHILE PARSING \n");
    printf("\n========================================\n\n");
    pp_rec(ast, 0);
    printf("\n-_-_-_-_-_-_-_-_-_-_\n\n");
}



#include <err.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "eval_ast.h"
#include "execution.h"
#include "options.h"
#include "special_vars.h"
#include "xalloc.h"

extern struct options *opt;

void exit_program(const char *msg)
{
    xfree_all();
    err(1, "%s", msg);
}

int execute(char **args)
{
    if (!args || !args[0])
        return 0;
    if (opt && opt->verbose)
    {
        fprintf(stdout, "Executing command -> %s\nWith args1 -> %s\n", args[0],
                args[1]);
    }
    struct ast *fc_ast;
    if (get_fc(args[0], &fc_ast) == 1)
    {
        return exec_func(args, fc_ast);
    }
    int index;
    if ((index = get_builins_index(args[0])) != -1)
    {
        int res = exec_builtin(index, args);
        return res;
    }
    pid_t pid = fork();
    if (pid == -1)
    {
        exit_program("Failed to fork");
    }
    if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            fprintf(stderr, "42SH: %s: not found\n", args[0]);
            exit(127);
        }
    }
    else
    {
        int wstatus;
        if (waitpid(pid, &wstatus, 0) == -1)
            exit_program("Failed to wait");
        return WEXITSTATUS(wstatus);
    }
    exit_program("Failed");
    return 0;
}

int exec_s_cmd(struct n_s_cmd *cmd)
{
    char **cmd_arg = expand_vars_vect(cmd->cmd_arg);
    if (!cmd->cmd_arg->len || !cmd_arg || !cmd_arg[0])
    {
        struct list_var_assign *tmp = cmd->vars;
        while (tmp)
        {
            char **value = expand_vars_vect(tmp->value);
            add_var(tmp->name, value[0]);
            tmp = tmp->next;
        }
    }
    else
        return (execute(cmd_arg));

    return 0;
}

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "eval_ast.h"
#include "execution.h"
#include "parser.h"
#include "xalloc.h"

int open_file(char *filename)
{
    FILE *temp = fopen(filename, "w");
    fclose(temp);
    int f = open(filename, O_WRONLY | O_CREAT, 644);
    if (f < 0)
    {
        xfree_all();
        err(2, "Failed to open file.");
    }
    return f;
}

int open_file_read(char *filename)
{
    int f = open(filename, O_RDONLY);
    if (f < 0)
    {
        xfree_all();
        err(2, "Failed to open file.");
    }
    return f;
}

void apply_redir(struct list_redir *redir, struct redir_info *redirInfo)
{
    char *end_ptr;
    redirInfo->io_number = strtol(redir->ionumber, &end_ptr, 10);
    switch (redir->redir_type)
    {
    case T_REDIR_1:
        redirInfo->file_fd = open_file(redir->word);
        redirInfo->temp_fd = dup(redirInfo->io_number);
        dup2(redirInfo->file_fd, redirInfo->io_number);
        return;
    case T_REDIR_I_1:
        redirInfo->file_fd = open_file_read(redir->word);
        redirInfo->temp_fd = dup(redirInfo->io_number);
        dup2(redirInfo->file_fd, redirInfo->io_number);
        return;
    default:
        err(127, "Unimplemented features");
    }
}

void unapply_redir(struct redir_info *redirInfo)
{
    close(redirInfo->file_fd);
    dup2(redirInfo->temp_fd, redirInfo->io_number);
    close(redirInfo->temp_fd);
}

struct redir_info *append_redir(struct redir_info *redirInfo,
                                struct redir_info *new)
{
    new->next = NULL;
    struct redir_info *temp = redirInfo;
    if (temp == NULL)
        return new;

    while (temp->next)
        temp = temp->next;

    temp->next = new;
    return redirInfo;
}

int exec_redirs(struct ast *ast, struct list_redir *listRedir)
{
    struct list_redir *temp = listRedir;
    struct redir_info *redirInfo = NULL;
    while (temp)
    {
        struct redir_info *new = xmalloc(sizeof(struct redir_info));
        new->file_fd = -1;
        new->temp_fd = -1;
        redirInfo = append_redir(redirInfo, new);
        apply_redir(temp, new);
        temp = temp->next;
    }
    int res = eval_ast(ast);
    struct redir_info *temp2 = redirInfo;
    while (temp2)
    {
        unapply_redir(temp2);
        struct redir_info *next = temp2->next;
        xfree(temp2);
        temp2 = next;
    }
    return res;
}

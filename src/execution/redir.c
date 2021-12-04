#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "eval_ast.h"
#include "execution.h"
#include "xalloc.h"
#include "xparser.h"

static int open_file(char *filename, char *flag)
{
    FILE *f = fopen(filename, flag);
    if (f == NULL)
    {
        xfree_all();
        err(2, "Failed to open file.");
    }
    int fd = fileno(f);
    return fd;
}

static int is_valid_fd(int fd)
{
    return fcntl(fd, F_GETFL) != -1 && errno != EBADF;
}

int apply_redir(struct list_redir *redir, struct redir_info *redirInfo)
{
    char *end_ptr;
    redirInfo->io_number = strtol(redir->ionumber, &end_ptr, 10);
    int temp;
    char *flag;
    char **word = expand_vars_vect(redir->word);
    switch (redir->redir_type)
    {
    case T_REDIR_PIPE:
    case T_REDIR_1:
        flag = "w";
        break;
    case T_REDIR_I_1:
        flag = "r";
        break;
    case T_REDIR_2:
        flag = "a";
        break;
    case T_REDIR_O_2:
        flag = "wr";
        break;
    case T_REDIR_A:
        temp = strtol(word[0], &end_ptr, 10);
        if (*end_ptr != '\0' || temp >= 10)
        {
            fprintf(stderr, "%d Bad fd number", temp);
            xfree_all();
            exit(2);
        }
        if (!is_valid_fd(temp))
        {
            fprintf(stderr, "%d Bad fd number", temp);
            return 2;
        }
        redirInfo->temp_fd = dup(redirInfo->io_number);

        if (redirInfo->temp_fd == -1 || dup2(temp, redirInfo->io_number) == -1)
        {
            fprintf(stderr, "%d Bad fd number", temp);
            return 2;
        }
        fcntl(redirInfo->temp_fd, F_SETFD, FD_CLOEXEC);
        return 0;


    default:
        err(127, "Unimplemented features");
    }

    redirInfo->file_fd = open_file(word[0], flag);
    redirInfo->temp_fd = dup(redirInfo->io_number);
    dup2(redirInfo->file_fd, redirInfo->io_number);
    fcntl(redirInfo->temp_fd, F_SETFD, FD_CLOEXEC);
    return 0;
}

void unapply_redir(struct redir_info *redirInfo)
{
    if (redirInfo->temp_fd != -1)
    {
        dup2(redirInfo->temp_fd, redirInfo->io_number);
        close(redirInfo->temp_fd);
    }
    if (redirInfo->file_fd != -1)
        close(redirInfo->file_fd);
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
    int res;
    while (temp)
    {
        struct redir_info *new = xmalloc(sizeof(struct redir_info));
        new->file_fd = -1;
        new->temp_fd = -1;
        redirInfo = append_redir(redirInfo, new);
        if ((res = apply_redir(temp, new)) != 0)
            return res;
        temp = temp->next;
    }
    res = eval_ast(ast);
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

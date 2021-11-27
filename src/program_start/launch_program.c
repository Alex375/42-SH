#include <parser.h>
#include <stdio.h>
#include <string.h>
#include <xalloc.h>
#include <unistd.h>
#include <err.h>

#include "options.h"
#include "read_script.h"

extern struct options *opt;

int launch_script(void)
{
    if (opt->nb_script == 0)
        return -1;
    size_t size;
    char *temp = read_script(opt->scripts[0], &size);
    if (opt->verbose)
        printf("Executing script \n%s\n-_-_-_-_-_-_-_-_-_-_-_-_-_\n\n", temp);

    exec_script(temp, size); // TODO get script exit code
    xfree(temp);
    return 0;
}

int launch_command(void)
{
    if (opt->nb_command == 0)
        return -1;
    if (opt->verbose)
        printf("Executing command \n\'%s\'\n", opt->commands[0]);
    exec_script(opt->commands[0], strlen(opt->commands[0]));//TODO get exec exit code
    return 0;
}

char *read_stdin(void)
{
    char *buffer = xcalloc(1, 2049);
    int total = 0;
    ssize_t r = 0;
    while ((r = read(STDIN_FILENO, buffer + total, 2048)) > 0)
    {
        total += r;
        xrecalloc(buffer, total + 2049);
    }
    if (r == -1)
    {
        xfree_all();
        err(1, "Failed to read stdin");
    }
    return buffer;
}

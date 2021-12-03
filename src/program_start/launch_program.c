#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <xalloc.h>
#include <xparser.h>

#include "options.h"
#include "read_script.h"
#include "vars.h"

struct options *opt = NULL;
struct vars_vect *vars = NULL;

int launch_script(void)
{
    if (opt->nb_script == 0)
        return -1;
    size_t size;
    char *temp = read_script(opt->scripts[0], &size);
    if (opt->verbose)
        printf("Executing script \n%s\n-_-_-_-_-_-_-_-_-_-_-_-_-_\n\n", temp);

    int res = exec_script(temp, size);
    xfree(temp);
    return res;
}

int launch_command(void)
{
    if (opt->nb_command == 0)
        return -1;
    if (opt->verbose)
        printf("Executing command \n\'%s\'\n", opt->commands[0]);
    return exec_script(opt->commands[0], strlen(opt->commands[0]));
}

char *read_stdin(void)
{
    char *buffer = xcalloc(1, 2049);
    int total = 0;
    ssize_t r;
    while ((r = read(STDIN_FILENO, buffer + total, 2048)) > 0)
    {
        total += r;
        buffer = xrecalloc(buffer, total + 2049);
    }
    if (r == -1)
    {
        xfree_all();
        err(1, "Failed to read stdin");
    }
    return buffer;
}

int launch_program(int argc, char **argv)
{
    vars = init_vars_vect();

    opt = xcalloc(1, sizeof(struct options));
    get_option(opt, argc, argv);
    if (opt->help)
    {
        print_usage();
        return 0;
    }

    int res = launch_script();
    if (res == -1)
        res = launch_command();

    if (res == -1)
    {
        char *read = read_stdin();
        if (opt->verbose)
            printf("Executing command \n\'%s\'\n", read);
        res = exec_script(read, strlen(read));
    }
    xfree_all();
    return res;
}

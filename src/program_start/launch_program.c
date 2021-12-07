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

    int prog_index = preparseopt(argc, argv);
    char **options = dupplicate(prog_index, argv);
    get_option(opt, prog_index, options);
    if (opt->help)
    {
        print_usage();
        return 0;
    }

    if (opt->script == NULL && prog_index < argc)
    {
        opt->script = read_script(argv[prog_index]);
        opt->argv = argv + prog_index;
        opt->argc = argc - prog_index;
    }

    if (opt->script == NULL)
    {
        opt->argv = argv;
        opt->argc = 1;
        opt->script = read_stdin();
    }
    if (opt->verbose)
        printf("Executing command \n\'%s\'\n", opt->script);
    int res = exec_script(opt->script, strlen(opt->script));
    xfree_all();
    return res;
}

#include <stdio.h>
#include <string.h>

#include "eval_ast.h"
#include "execution.h"
#include "options.h"
#include "parser.h"
#include "read_script.h"
#include "xalloc.h"

struct options *opt = NULL;

int main(int argc, char **argv)
{
    opt = xcalloc(1, sizeof(struct options));
    get_option(opt, argc, argv);
    if (opt->help)
    {
        print_usage();
        return 0;
    }

    for (size_t i = 0; i < opt->nb_script; i++)
    {
        size_t size;
        char *temp = read_script(opt->scripts[i], &size);
        if (opt->verbose)
            printf("Executing script \n%s\n-_-_-_-_-_-_-_-_-_-_-_-_-_\n\n",
                   temp);

        exec_script(temp, size);
        xfree(temp);
    }

    for (size_t i = 0; i < opt->nb_command; i++)
    {
        if (opt->verbose)
            printf("Executing command \n\'%s\'\n", opt->commands[i]);
        exec_script(opt->commands[i], strlen(opt->commands[i]));
    }
    xfree_all();
    return 0;
}

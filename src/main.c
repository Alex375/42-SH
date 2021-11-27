#include <stdio.h>
#include <string.h>

#include "launch_program.h"
#include "options.h"
#include "parser.h"
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

    int res = launch_script();
    if (res == -1)
        res = launch_command();

    if (res == -1)
    {
        char *read = read_stdin();
        if (opt->verbose)
            printf("Executing command \n\'%s\'\n", opt->commands[0]);
        res = exec_script(read,
                          strlen(read));
    }
    xfree_all();
    return res;
}

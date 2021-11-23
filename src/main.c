#include <err.h>
#include <stdio.h>

#include "xalloc.h"
#include "options.h"
#include "read_script.h"


int main(int argc, char **argv)
{
    struct options *opt = xcalloc(1, sizeof(struct options));
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
        printf("%s\n", temp);
        // TODO: call program
        xfree(temp);
    }

    for (size_t i = 0; i < opt->nb_command; i++)
    {
        printf("%s\n", opt->commands[i]);
        // TODO: call program
    }
    xfree_all();
    return 0;
}

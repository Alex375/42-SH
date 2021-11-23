#include "options.h"

#include <getopt.h>
#include <stdio.h>

#include "xmalloc.h"

void print_usage(void)
{
    printf("Usage : 42sh [OPTIONS] [SCRIPT] [ARGUMENTS .."
           ".]\n\nOptions\n\t-c, --commands <commands> Directly interpret "
           "the argument as a shell script\n");
}

int get_scripts(int argc, char **argv, struct options *options)
{
    while (optind < argc)
    {
        options->scripts =
            realloc(options->scripts, ++options->nb_script * sizeof(char *));
        options->scripts[options->nb_script - 1] = argv[optind++];
    }
    if (options->nb_script == 0 && options->commands == NULL)
    {
        fprintf(stderr, "No arguments\n");
        print_usage();
        return 1;
    }
    return 0;
}

int get_option(struct options *options, int argc, char **argv)
{
    struct option long_options[] = { { "commands", required_argument, 0, 'c' },
                                     { "pretty_print", no_argument, 0, 'p' },
                                     { "help", no_argument, 0, 'h' },
                                     { 0, 0, 0, 0 } };
    int c;

    opterr = 0;
    int long_index = 0;
    while ((c = getopt_long(argc, argv, "c:ph", long_options, &long_index))
           != -1)
    {
        switch (c)
        {
        case 'c':
            options->commands =
                realloc(options->commands, ++options->nb_command * sizeof(char *));
            options->commands[options->nb_command - 1] = optarg;
            break;
        case 'p':
            options->print = 1;
            break;
        case 'h':
            options->help = 1;
            break;
        default:
            fprintf(stderr, "Wrong argument\n");
            print_usage();
            return 1;
        }
    }

    return get_scripts(argc, argv, options);
}

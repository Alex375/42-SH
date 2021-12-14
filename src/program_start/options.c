#include "options.h"

#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include "xalloc.h"
#include "xparser.h"

void print_usage(void)
{
    printf("Usage : 42sh [OPTIONS] [SCRIPT] [ARGUMENTS .."
           ".]\n\nOptions\n\t-c, --commands <commands> Directly interpret "
           "the argument as a shell script\n\t-p, --pretty_print        Print "
           "the ast in "
           "a pretty way\n\t-v, --verbose             Display some additional "
           "information\n");
}

void get_option(struct options *options, int argc, char **argv)
{
    struct option long_options[] = { { "commands", required_argument, 0, 'c' },
                                     { "pretty_print", no_argument, 0, 'p' },
                                     { "verbose", no_argument, 0, 'v' },
                                     { "help", no_argument, 0, 'h' },
                                     { 0, 0, 0, 0 } };
    int c;

    opterr = 0;
    int long_index = 0;
    while ((c = getopt_long(argc, argv, "c:pvh", long_options, &long_index))
           != -1)
    {
        switch (c)
        {
        case 'c':
            options->argv = argv;
            options->argc = 1;
            options->script = optarg;
            break;
        case 'p':
            options->print = 1;
            break;
        case 'h':
            options->help = 1;
            break;
        case 'v':
            options->verbose = 1;
            break;
        case '?':
            xfree_all();
            err(2, "Illegal option");
        default:
            fprintf(stderr, "Wrong argument\n");
            print_usage();
            xfree_all();
            exit(1);
        }
    }

    if (optind < argc)
    {
        xfree_all();
        err(2, "Illegal option");
    }
}

int preparseopt(int argc, char **argv)
{
    int i = 1;
    while (i < argc)
    {
        if (argv[i][0] == '-' && strlen(argv[i]) > 1)
        {
            if (strcmp(argv[i], "-c") == 0)
                return i + 2;
            i++;
            continue;
        }
        break;
    }
    return i;
}

char **dupplicate(int argc, char **argv)
{
    char **res = NULL;
    int i = 0;
    while (i < argc)
    {
        res = xrealloc(res, (i + 1) * sizeof(char *));
        res[i] = xcalloc(strlen(argv[i]) + 1, sizeof(char));
        strcpy(res[i], argv[i]);
        i++;
    }
    return res;
}

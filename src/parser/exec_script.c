#include <err.h>
#include <errno.h>
#include <stddef.h>

#include "eval_ast.h"
#include "handle_ast.h"
#include "options.h"
#include "read_script.h"
#include "special_vars.h"
#include "vars.h"
#include "xalloc.h"
#include "xparser.h"

extern struct options *opt;

int exec_script(char *script, size_t size, int set_var)
{
    if (set_var)
        set_special_vars(opt->argc, opt->argv);
    struct ast *ast;
    errno = 0;

    int res = 0;

    lexer_start(script, size, -1);
    while (errno != ERROR_EMPTY_EOF)
    {
        errno = 0;
        ast = parse_input();
        if (opt && opt->print)
            ast_pretty_print(ast);
        if (errno != 0)
        {
            if (errno == ERROR_PARSING)
            {
                xfree_all();
                err(2, "Error while parsing shell command.");
            }
            break;
        }
        else if (ast)
            res = eval_ast(ast);

        handle_rec(ast, H_FREE);
    }
    lexer_reset();
    if (set_var)
        free_context();
    return res;
}

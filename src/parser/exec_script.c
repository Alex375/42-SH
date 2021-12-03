#include <err.h>
#include <errno.h>
#include <stddef.h>

#include "eval_ast.h"
#include "options.h"
#include "read_script.h"
#include "xalloc.h"
#include "xparser.h"
#include "vars.h"

extern struct options *opt;

int exec_script(char *script, size_t size)
{
    struct ast *ast;
    errno = 0;

    struct vars_vect *vars = init_vars_vect();

    int res = 0;

    lexer_start(script, size);
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
            res = eval_ast(ast, vars);
    }
    lexer_reset();
    return res;
}

#include <err.h>
#include <errno.h>
#include <stddef.h>

#include "eval_ast.h"
#include "options.h"
#include "parser.h"
#include "read_script.h"
#include "xalloc.h"

extern struct options *opt;

int exec_script(char *script, size_t size)
{
    struct ast *ast;
    errno = 0;

    int res = 0;

    lexer_start(script, size);
    while (errno != ERROR_EMPTY_EOF)
    {
        errno = 0;
        ast = parse_input();
        if (opt->print)
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
    }
    lexer_reset();
    return res;
}

struct ast *start_parse(char *script, size_t size)
{
    lexer_start(script, size);

    struct ast *res = parse_input();

    lexer_reset();

    return res;
}

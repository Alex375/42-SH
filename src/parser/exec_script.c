#include <errno.h>
#include <err.h>
#include <stddef.h>

#include "parser.h"
#include "eval_ast.h"
#include "xalloc.h"
#include "options.h"
#include "read_script.h"

extern struct options *opt;

void exec_script(char *script, size_t size)
{
    struct ast *ast;
    errno = 0;

    //struct pipeline *pipeline = xcalloc(1, sizeof(struct pipeline));

    lexer_start(script, size);
    while (errno != ERROR_EMPTY_EOF)
    {
        errno = 0;
        //pipeline->out = -1;
        ast = parse_input();
        if (opt->print)
            ast_pretty_print(ast);
        if (errno != 0)
        {
            if (errno == ERROR_PARSING)
            {
                xfree_all();
                err(127, "Error while parsing shell command.");
            }
            break;
        }
        else if (ast)
            eval_ast(ast);
    }
    lexer_reset();
   // xfree(pipeline);
}

struct ast *start_parse(char *script, size_t size)
{
    lexer_start(script, size);

    struct ast *res = parse_input();

    lexer_reset();

    return res;
}

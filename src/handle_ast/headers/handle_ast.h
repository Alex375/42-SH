#ifndef INC_42_SH_HANDLE_AST_H
#define INC_42_SH_HANDLE_AST_H

enum handle
{
    H_FREE,
    H_DUP
};

void free_char_star_star(char **lst);
char **dup_char_star_star(char **lst);

struct ast *handle_rec(struct ast *ast, enum handle h);
struct ast *handle_rec2(struct ast *ast, enum handle h);
struct ast *handle_rec3(struct ast *ast, enum handle h);

#endif // INC_42_SH_HANDLE_AST_H

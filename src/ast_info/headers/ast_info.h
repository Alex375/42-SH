#ifndef INC_42_SH_AST_INFO_H
#define INC_42_SH_AST_INFO_H

enum ast_info_type
{
    A_NOTHING,
    A_BREAK,
    A_CONTINUE
};

struct ast_info
{
    int depth;
    enum ast_info_type type;
    int code;
};

void inc_depth();

void set_b_c(int code, enum ast_info_type type);

enum ast_info_type b_c_is_done();

enum ast_info_type is_breaking();

#endif // INC_42_SH_AST_INFO_H

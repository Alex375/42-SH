#Project definition
project(
    '42sh',
    'c',
    version: 'dev',
    default_options: [
        'c_std=c99',
        'warning_level=3',
        'werror=true',
        'b_sanitize=address'
    ],
)

# print a summary of the build configuration before each build starts
summary({
    'optimization': get_option('optimization'),
    'debug': get_option('debug'),
    'b_sanitize': get_option('b_sanitize'),
    'warning_level': get_option('warning_level'),
    'werror': get_option('werror')
})



#Compile options
gcc = meson.get_compiler('c')
cflags = ['-pedantic']
add_project_arguments(cflags, language: 'c')

#Sources
sources = [

    'src/execution/xexecution.c',
    'src/execution/pipe.c',
    'src/execution/redir.c',
    'src/execution/funcx.c',
    'src/execution/command_sub.c',
    'src/execution/subshell.c',
    'src/execution/case.c',
    'src/data_types/token_vec.c',
    'src/data_types/xstring.c',
    'src/builtins/builtins_dispatch.c',
    'src/builtins/echo.c',
    'src/builtins/exit.c',
    'src/builtins/unset.c',
    'src/builtins/cd.c',
    'src/builtins/dot.c',
    'src/builtins/export.c',
    'src/builtins/pwd.c',
    'src/builtins/break_continue.c',
    'src/parser/ast_pretty_print.c',
    'src/parser/parser_cmds.c',
    'src/parser/builds.c',
    'src/parser/builds2.c',
    'src/parser/tools_parse.c',
    'src/parser/parser_shell_cmds.c',
    'src/parser/exec_script.c',
    'src/parser/parse_case.c',
    'src/parser/parser.c',
    'src/parser/xvector_tokens.c',
    'src/program_start/options.c',
    'src/program_start/read_script.c',
    'src/program_start/launch_program.c',
    'src/program_start/special_vars.c',
    'src/xalloc/xalloc.c',
    'src/io/xstrdup.c',
    'src/lexer/lexer_start.c',
    'src/lexer/lexer_context.c',
    'src/lexer/lexer.c',
    'src/lexer/lexer_keywords.c',
    'src/lexer/lexer_squote.c',
    'src/lexer/lexer_redir.c',
    'src/lexer/lexer_dquote.c',
    'src/lexer/lexer_operations.c',
    'src/lexer/lexer_for.c',
    'src/lexer/lexer_squote.c',
    'src/lexer/lexer_var.c',
    'src/lexer/recognize_words.c',
    'src/lexer/lexer_command.c',
    'src/lexer/lexer_command_sub.c',
    'src/lexer/lexer_fun.c',
    'src/eval_ast/eval_ast.c',
    'src/vars/expand_vars.c',
    'src/lexer/lexer_fun.c',
    'src/vars/xvars.c',
    'src/vars/xvars2.c',
    'src/vars/context.c',
    'src/handle_ast/handle_ast.c',
    'src/func/func.c',
    'src/ast_info/ast_info.c',
    'src/evalexpr/adapt.c',
    'src/evalexpr/eval.c',
    'src/evalexpr/evalexpr.c',
    'src/evalexpr/launch.c',
    'src/evalexpr/rpn.c',
]

includes = [
    'src/execution/headers',
    'src/parser/headers',
    'src/lexer/headers',
    'src/eval_ast/headers',
    'src/io/headers',
    'src/xalloc/headers',
    'src/program_start/headers',
    'src/data_types/headers',
    'src/builtins/headers',
    'src/vars/headers',
    'src/func/headers',
    'src/handle_ast/headers',
    'src/ast_info/headers',
    'src/evalexpr/headers'
]

#42SH binary
sh_sources = sources + ['src/main.c']
sh_bin = executable('42sh', sh_sources, include_directories: includes)

if get_option('doc')
    subdir('doc')
endif

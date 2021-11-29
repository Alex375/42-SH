#Project definition
project(
    '42sh',
    'c',
    version: 'dev',
    default_options: [
        'c_std=c99',
        'warning_level=3'
    ],
)


#Compile options
gcc = meson.get_compiler('c')
cflags = ['-pedantic']
add_project_arguments(cflags, language: 'c')

#Sources
sources = [
    'src/execution/execution.c',
    'src/execution/pipe.c',
    'src/data_types/token_vec.c',
    'src/data_types/xstring.c',
    'src/builtins/builtins_dispatch.c',
    'src/builtins/echo.c',
    'src/parser/ast_pretty_print.c',
    'src/parser/parser_cmds.c',
    'src/parser/builds.c',
    'src/parser/tools_parse.c',
    'src/parser/parser_shell_cmds.c',
    'src/parser/exec_script.c',
    'src/parser/parser.c',
    'src/program_start/options.c',
    'src/program_start/read_script.c',
    'src/program_start/launch_program.c',
    'src/xalloc/xalloc.c',
    'src/io/xstrdup.c',
    'src/lexer/lexer_start.c',
    'src/lexer/lexer_context.c',
    'src/lexer/lexer.c',
    'src/lexer/lexer_keywords.c',
    'src/lexer/lexer_squote.c',
    'src/lexer/lexer_redir.c',
    'src/lexer/recognize_words.c',
    'src/lexer/lexer_command.c',
    'src/eval_ast/eval_ast.c',
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
    'src/builtins/headers'
]

#42SH binary
sh_sources = sources + ['src/main.c']
sh_bin = executable('42sh', sh_sources, include_directories: includes)

#criterion depencie
criterion_dep = gcc.find_library('criterion')

#Test lexer binary
test_lexer_sources = sources + ['tests/unit_tests/lexer_test.c']
test_lexer_bin = executable('test_lexer', test_lexer_sources, include_directories: includes, dependencies: criterion_dep)


#Project settings
cmake_minimum_required(VERSION 3.19)
project(42_SH C)
add_compile_options(-Werror -Wall -Wextra -pedantic)
set(CMAKE_C_STANDARD 99)

#Include headers
include_directories(src/xalloc/headers)
include_directories(src/io/headers)
include_directories(src/lexer/headers)
include_directories(src/parser/headers)
include_directories(src/eval_ast/headers)
include_directories(src/data_types/headers)
include_directories(src/program_start/headers)
include_directories(src/execution/headers)
include_directories(src/builtins/headers)

file(GLOB SRC src/xalloc/*.c src/io/*.c src/lexer/*.c src/parser/*.c src/eval_ast/*.c src/program_start/*c src/execution/*.c src/data_types/*.c src/builtins/*.c)

add_executable(42SH ${SRC} src/main.c)

#Test executable
add_executable(lexer_test ${SRC} tests/unit_tests/lexer_test.c)

add_executable(echo_test ${SRC} tests/unit_tests/echo_tests/echo_tests.c)
add_executable(echo_main ${SRC} tests/unit_tests/echo_tests/echo_main.c)

target_link_libraries(lexer_test PUBLIC -lcriterion)
target_link_libraries(echo_test PUBLIC -lcriterion)

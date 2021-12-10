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
include_directories(src/vars/headers)
include_directories(src/func/headers)
include_directories(src/handle_ast/headers)


file(GLOB SRC src/xalloc/*.c src/io/*.c src/lexer/*.c src/parser/*.c src/eval_ast/*.c src/program_start/*c src/execution/*.c src/data_types/*.c src/builtins/*.c src/xfnmatch/*.c src/vars/*.c src/func/*.c src/handle_ast/*.c)

if (APPLE)
    include_directories(src/xfnmatch)
    add_executable(42SH ${SRC} src/main.c src/xfnmatch/xfnmatch.c)
    target_link_libraries(42SH PRIVATE -fsanitize=address)
else()
    add_executable(42SH ${SRC} src/main.c)
    target_link_libraries(42SH PUBLIC -fsanitize=address)
endif()

#Test executable
add_executable(lexer_test ${SRC} tests/unit_tests/lexer_test.c)
target_link_libraries(lexer_test PRIVATE -fsanitize=address)
target_link_libraries(lexer_test PUBLIC -lcriterion)

add_executable(echo_test ${SRC} tests/unit_tests/echo_tests/echo_tests.c)
target_link_libraries(echo_test PUBLIC -lcriterion)

add_executable(echo_main ${SRC} tests/unit_tests/echo_tests/echo_main.c)

add_executable(export_test ${SRC} tests/unit_tests/export_test.c)
target_link_libraries(export_test PRIVATE -fsanitize=address)

add_executable(dot_test ${SRC} tests/unit_tests/dot_test.c)
target_link_libraries(export_test PRIVATE -fsanitize=address)



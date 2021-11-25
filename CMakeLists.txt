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

#Add every source files of the project in the SRC variable
file(GLOB SRC src/execution/*.c src/xalloc/*.c src/io/*.c src/lexer/*.c
        src/program_start/*c
        src/data_types/*.c src/parser/*.c src/eval_ast/*.c)

#Main executable
add_executable(42_SH ${SRC} src/main.c)

#Test executable
add_executable(lexer_test ${SRC} tests/unit_tests/lexer_test.c)
target_link_libraries(lexer_test PUBLIC -lcunit)

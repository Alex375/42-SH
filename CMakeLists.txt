cmake_minimum_required(VERSION 3.19)
project(42_SH C)

add_compile_options(-Werror -Wall -Wextra -pedantic)

include_directories(src/xmalloc/headers)
include_directories(src/program_start/headers)

set(CMAKE_C_STANDARD 99)

file(GLOB SRC src/xmalloc/*.c src/main.c src/program_start/*c)



add_executable(42_SH ${SRC})

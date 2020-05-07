cmake_minimum_required(VERSION 2.6)
project(RmbRT_Language_Compiler)

# Set the C standard used by the RmbRT Language Compiler.
set(CMAKE_C_FLAGS "-std=gnu11 -DRLC_DEBUG -Werror -Wall -Wextra -Wfatal-errors -g")

file(GLOB_RECURSE rlc_sources ./src/*.c)
add_executable(rmbrtbc ${rlc_sources})


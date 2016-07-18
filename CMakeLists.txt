cmake_minimum_required(VERSION 2.6)
project(RmbRT_Language_Compiler)

# Set the C standard used by the RmbRT Language Compiler.
set(CMAKE_C_FLAGS "-std=c99 -Wfatal-errors -Werror")

# Select all source files.
file(GLOB_RECURSE rlc_sources ./src/*.c)
# Add them to the RmbRT Language Compiler library.
add_library(rlc ${rlc_sources})

# Creates an include directory containing all header files used in the RmbRT Language Compiler.
# Add /rlc/include/ to your include directories and access the files via #include <rlc/*>
file(COPY "src/" DESTINATION "include/rlc/" FILES_MATCHING PATTERN "*.h")
# Set up the rlc include directory.
include_directories(./include/)
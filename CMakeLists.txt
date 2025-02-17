cmake_minimum_required(VERSION 3.25)
project(asm C)

set(CMAKE_C_STANDARD 90)

add_executable(asm main.c pre_asm.c pre_asm.h utils.c utils.h first_pass.c first_pass.h globals.h symbol_structs.c symbol_structs.h second_pass.c second_pass.h output_files.c output_files.h)
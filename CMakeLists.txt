cmake_minimum_required(VERSION 3.15)
project(minibash_de_so C)

set(CMAKE_C_STANDARD 11)

add_executable(minibash
        shell.c
        utils.c
        utils.h
        )

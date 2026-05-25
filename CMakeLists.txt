cmake_minimum_required(VERSION 3.16)

project(Assembler LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(Assembler
    src/main.c
    src/core/table.h
    src/core/table.c
    src/core/general.c
    src/core/general.h
)

target_include_directories(Assembler
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
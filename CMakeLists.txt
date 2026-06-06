cmake_minimum_required(VERSION 3.16)

project(Assembler LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(MainLibrary
    src/instruction.c
    src/general.h
    src/instruction.h
    src/table.h
    src/procedures.h
    src/procedures.c
    src/table.c
)

target_include_directories(MainLibrary
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(Assembler
    src/main.c
)

target_link_libraries(Assembler
    PRIVATE MainLibrary
)
enable_testing()
add_subdirectory(tests)
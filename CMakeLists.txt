cmake_minimum_required(VERSION 3.10.0)
project(dice_algebra_calculator VERSION 0.1.0 LANGUAGES C)

set(SOURCES
    src/main.c
    src/lexer.c
    src/dice_error.c
    src/parser.c
)

add_executable(dice_algebra_calculator ${SOURCES})

target_include_directories(dice_algebra_calculator PRIVATE ${CMAKE_SOURCE_DIR}/src)

include(CTest)
enable_testing()


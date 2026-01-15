cmake_minimum_required(VERSION 3.10.0)
project(dice_algebra_calculator VERSION 0.1.0 LANGUAGES C)

set(SOURCES
    src/main.c
    src/dice_error.c
    src/executor.c
    src/lexer.c
    src/parser.c
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)

add_executable(dice_algebra_calculator ${SOURCES})

target_include_directories(dice_algebra_calculator PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(dice_algebra_calculator PkgConfig::GLIB)

include(CTest)
enable_testing()


cmake_minimum_required (VERSION 2.8)
project(cw08zad1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_POSIX_C_SOURCE=200809L")

add_subdirectory("src")

add_executable(zad1a src/main.c)
add_executable(zad1b src/main.c)
add_executable(zad1c src/main.c)
add_executable(zad1gen src/generator.c)

target_compile_definitions(zad1a PUBLIC ZADANIE1A)
target_compile_definitions(zad1b PUBLIC ZADANIE1B)
target_compile_definitions(zad1c PUBLIC ZADANIE1C)

target_link_libraries(zad1a PUBLIC pthread)
target_link_libraries(zad1b PUBLIC pthread)
target_link_libraries(zad1c PUBLIC pthread)


set_target_properties(zad1a zad1b zad1c zad1gen
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
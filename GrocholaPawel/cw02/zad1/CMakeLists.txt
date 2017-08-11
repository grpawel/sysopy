cmake_minimum_required (VERSION 2.8)
project(cw02zad1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter")

add_subdirectory("src")

add_executable(zad1 ${sources} ${headers})

include_directories("../common/options/src")
find_library(OPTIONS options_static ../common/options/lib)
target_link_libraries(zad1 ${OPTIONS})

set_target_properties(zad1
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
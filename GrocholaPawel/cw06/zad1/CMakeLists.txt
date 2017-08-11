cmake_minimum_required (VERSION 2.8)
project(cw06zad1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_POSIX_C_SOURCE=200809L")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_XOPEN_SOURCE")

add_subdirectory("src")

set(server_sources ${sources})
set(client_sources ${sources})

list(REMOVE_ITEM server_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/client.c)
list(REMOVE_ITEM client_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/server.c)
add_executable(zad1server ${server_sources} ${headers})
add_executable(zad1client ${client_sources} ${headers})

set_target_properties(zad1server zad1client
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
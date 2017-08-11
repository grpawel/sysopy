cmake_minimum_required (VERSION 2.8)
project(cw10zad2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_POSIX_C_SOURCE=200809L")

add_subdirectory("src")

set(server_sources ${sources})
set(client_sources ${sources})

list(REMOVE_ITEM server_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/client.c)
list(REMOVE_ITEM client_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/server.c)

add_executable(zad2server ${server_sources} ${headers})
add_executable(zad2client ${client_sources} ${headers})

target_link_libraries(zad2server PUBLIC pthread)
target_link_libraries(zad2client PUBLIC pthread)

set_target_properties(zad2server zad2client
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
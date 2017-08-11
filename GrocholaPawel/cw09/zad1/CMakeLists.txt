cmake_minimum_required (VERSION 2.8)
project(cw09zad1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_POSIX_C_SOURCE=200809L")

add_subdirectory("src")

set(zad1_sources ${sources})
set(zad2_sources ${sources})

list(REMOVE_ITEM zad1_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/threads_sem_posix.c)
list(REMOVE_ITEM zad2_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/threads_mutex_pthread.c)

add_executable(zad1 ${zad1_sources} ${headers})
add_executable(zad2 ${zad2_sources} ${headers})

target_link_libraries(zad1 PUBLIC pthread)
target_link_libraries(zad2 PUBLIC pthread)

set_target_properties(zad1 zad2
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
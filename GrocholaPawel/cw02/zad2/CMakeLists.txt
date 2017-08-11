cmake_minimum_required (VERSION 2.8)
project(cw02zad2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_XOPEN_SOURCE=500")

add_subdirectory("src")

add_executable(zad2_dirstat ${sources} ${headers})
add_executable(zad2_nftw ${sources} ${headers})

target_compile_definitions(zad2_dirstat PRIVATE DIRSTAT)
target_compile_definitions(zad2_nftw PRIVATE NFTW)

set_target_properties(zad2_dirstat zad2_nftw 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
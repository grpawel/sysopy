cmake_minimum_required (VERSION 2.8)

project(cw01zad3b)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

set(CMAKE_C_FLAGS_O0 " -O0")
set(CMAKE_C_FLAGS_O1 " -O1")
set(CMAKE_C_FLAGS_O2 " -O2")
set(CMAKE_C_FLAGS_O3 " -O3")
set(CMAKE_C_FLAGS_OS " -OS")

add_subdirectory(../zad1 ./lib)

add_executable(benchmark_static ../zad2/address_book_benchmark.c)
target_link_libraries(benchmark_static address_book_static)

add_executable(benchmark_shared ../zad2/address_book_benchmark.c)
target_link_libraries(benchmark_shared address_book_shared)
#[[
add_executable(benchmark_dynamic ../zad2/address_book_benchmark.c)
target_link_libraries(benchmark_dynamic ${CMAKE_DL_LIBS})
target_compile_definitions(benchmark_dynamic PRIVATE DYNAMICALLY_LOAD)
]]

enable_testing()
add_test(static benchmark_static)
add_test(shared benchmark_shared)
#add_test(dynamic benchmark_dynamic)
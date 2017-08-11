cmake_minimum_required (VERSION 2.8)
project(cw03test)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")


add_executable(test_return "test_return.c")
add_executable(test_env "test_env.c")
add_executable(test_cpu "test_cpu.c")
target_link_libraries(test_cpu "m")
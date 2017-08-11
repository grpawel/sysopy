cmake_minimum_required (VERSION 2.8)
project(cw07zad2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_POSIX_C_SOURCE=200809L")

add_subdirectory("src")

set(barber_sources ${sources})
set(customers_sources ${sources})

list(REMOVE_ITEM barber_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/customers.c)
list(REMOVE_ITEM customers_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/barber.c)

add_executable(zad2barber ${barber_sources} ${headers})
add_executable(zad2customers ${customers_sources} ${headers})

target_link_libraries(zad2barber PUBLIC pthread)
target_link_libraries(zad2customers PUBLIC pthread)

target_link_libraries(zad2barber PUBLIC rt)
target_link_libraries(zad2customers PUBLIC rt)

set_target_properties(zad2barber zad2customers
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
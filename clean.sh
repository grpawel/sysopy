#!/bin/bash
cw="GrocholaPawel/"$1
find "$cw" -type f -executable -exec sh -c 'test "$(head -c 2 "$1")" != "#!"' sh {} \; -delete
find "$cw" -name "CMakeCache.txt" -type f -delete
find "$cw" -name "cmake_install.cmake" -type f -delete
find "$cw" -name "Makefile" -type f -delete
find "$cw" -name "*.o" -type f -delete
find "$cw" -name "lib*.a" -type f -delete
find "$cw" -name "lib*.so" -type f -delete
find "$cw" -path '*/CMakeFiles/*' -delete
find "$cw" -name "CMakeFiles" -type d -delete
find "$cw" -type d -empty -delete
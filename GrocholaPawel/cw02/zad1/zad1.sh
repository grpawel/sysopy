#!/bin/bash
./test_command.sh shuffle 131072 4 test/file512k
./test_command.sh shuffle 32768 4 test/file128k
./test_command.sh shuffle 8192 512 test/file4M
./test_command.sh shuffle 1024 512 test/file512k
./test_command.sh shuffle 1024 4096 test/file4M
./test_command.sh shuffle 128 4096 test/file512k
./test_command.sh shuffle 512 8192 test/file4M
./test_command.sh shuffle 64 8192 test/file512k

./test_command.sh sort 1024 4 test/file4k
./test_command.sh sort 256 4 test/file1k
./test_command.sh sort 1024 512 test/file512k
./test_command.sh sort 256 512 test/file128k
./test_command.sh sort 1024 4096 test/file4M
./test_command.sh sort 256 4096 test/file1M
./test_command.sh sort 512 8192 test/file4M
./test_command.sh sort 128 8192 test/file1M
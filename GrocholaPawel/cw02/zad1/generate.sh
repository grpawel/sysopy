#!/bin/bash
if [ ! -d "test" ]; then
  mkdir test
fi
bin/zad1 generate lib test/file4M 4 1048576 #4194304
bin/zad1 generate lib test/file1M 1 1048576
bin/zad1 generate lib test/file512k 4 131072 #524288
bin/zad1 generate lib test/file128k 1 131072
bin/zad1 generate lib test/file4k 4 1024 #4096
bin/zad1 generate lib test/file1k 1 1024

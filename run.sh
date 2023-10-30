#!/bin/bash
cd llvm-pass-skeleton
mkdir build
cd build
cmake ..
make
cd ..
clang -g -fpass-plugin=`echo build/skeleton/SkeletonPass.*` test/test1.c
# echo "test1 completed.\n\n\n\n\n"
# clang -fpass-plugin=`echo build/skeleton/SkeletonPass.*` test/test2.c
# echo "test2 completed.\n\n\n\n\n"
# clang -g -fpass-plugin=`echo build/skeleton/SkeletonPass.*` test/test3.c
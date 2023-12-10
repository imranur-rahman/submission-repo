#!/bin/bash
# cd llvm-pass-skeleton
mkdir build
cd build
cmake ..
make
cd ..
echo -e "\ntest1 \n"
clang -g -O0 -fpass-plugin=`echo build/skeleton/SkeletonPass.*` test/test1.c
echo -e "test1 completed.\n"
echo -e "\ntest2 \n"
clang -g -O0 -fpass-plugin=`echo build/skeleton/SkeletonPass.*` test/test2.c
echo -e "test2 completed.\n"
# clang -g -O0 -fpass-plugin=`echo build/skeleton/SkeletonPass.*` test/test3.c
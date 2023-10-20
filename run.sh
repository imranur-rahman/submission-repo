#!/bin/bash
cd llvm-pass-skeleton
mkdir build
cd build
cmake ..
make
cd ..
clang -fpass-plugin=`echo build/skeleton/SkeletonPass.*` test/test1.c
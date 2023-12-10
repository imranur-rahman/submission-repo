#!/bin/bash
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
echo -e "\ntest2 completed.\n"
echo -e "\ntest3 \n"
clang -g -O0 -fpass-plugin=`echo build/skeleton/SkeletonPass.*` test/test3.c
echo -e "\ntest3 completed.\n"
echo -e "\ntest4 \n"
clang++ -stdlib=libstdc++ -g -O0 -fpass-plugin=`echo build/skeleton/SkeletonPass.*` test/test5.cpp
echo -e "\ntest4 completed.\n"
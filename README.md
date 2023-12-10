# CSC 512 Part 2 & 3


Install LLVM:

    $ wget --no-check-certificate -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
    $ add-apt-repository 'deb http://apt.llvm.org/bionic/   llvm-toolchain-bionic-10  main'
    $ sudo apt update
    $ sudo apt install llvm

Install Clang:

    $ sudo apt install clang

Install cmake:

    $ sudo apt install cmake

Install libstdc++ (for running the last test case):

    $ sudo apt install libstdc++-12-dev
    $ sudo apt-get install libc++-dev

Build and Run (shortcut to run inside of the folder):

    $ chmod +x run.sh
    $ ./run.sh

Build:

    $ cd submission-repo
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ cd ..

Run:

    $ clang -g -O0 -fpass-plugin=`echo build/skeleton/SkeletonPass.*` something.c

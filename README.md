# llvm-pass-skeleton

A completely useless LLVM pass.
It's for LLVM 17.

Install LLVM:

    $ wget --no-check-certificate -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
    $ add-apt-repository 'deb http://apt.llvm.org/bionic/   llvm-toolchain-bionic-10  main'
    $ sudo apt update
    $ sudo apt install llvm

Install Clang:

    $ sudo apt install clang

Install cmake:

    $ sudo apt install cmake

Build and Run (shortcut):

    $ chmod +x run.sh
    $ ./run.sh

Build:

    $ cd llvm-pass-skeleton
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ cd ..

Run:

    $ clang -fpass-plugin=`echo build/skeleton/SkeletonPass.*` something.c

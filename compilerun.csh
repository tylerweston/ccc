#!/bin/csh
echo "================================================================================"
echo valgrind output:
valgrind --leak-check=full ./build/src/ece467c 4 ./codegen_tests/$1.c
echo "================================================================================"
echo clang output:
clang ./codegen_tests/$1.c.ll -L/u/a/keithste/ece467lab4/build/src -lece467rt -o ./objfiles/$1.o
echo "================================================================================"
echo executable output:
./objfiles/$1.o
set rc=$?
echo "================================================================================"
echo "executable return code:"
echo $rc
echo "================================================================================"
echo "source:"
cat ./codegen_tests/$1.c
echo "================================================================================"
echo llvm ir:
cat ./codegen_tests/$1.c.ll
echo "================================================================================"

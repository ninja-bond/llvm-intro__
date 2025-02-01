#!/usr/bin/bash

clang++ emit-ir.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o emit-ir.out
./emit-ir.out


printf "\n"
lli out.ll

echo $?
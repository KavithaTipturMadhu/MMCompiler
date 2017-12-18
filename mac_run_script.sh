#!/bin/bash



## declare an array variable
declare -a testCases=("startupglowplugs" "startupstarter" "startuppump_original" "startupoil" "runningregulation_dl" "runningregulation_nh" "runningregulation_nl" "fib")

## now loop through the above array
for i in "${testCases[@]}"
do
   echo "###########Running test case -" $i"###############"
   # or do whatever with individual element of the array
  $LLVM_BIN/clang -emit-llvm -target redefine $LLVM_TESTS/ECU_KERNELS/Compilation_Error/$i.c  -S -o $LLVM_TESTS/ECU_KERNELS/Compilation_Error/$i.ll
  $LLVM_BIN/opt -mem2reg  -load $LLVM_BIN/../lib/HyperOpCreationPass.dylib -HyperOpCreationPass $LLVM_TESTS/ECU_KERNELS/Compilation_Error/$i.ll -S -o $LLVM_TESTS/ECU_KERNELS/Compilation_Error/$i.hop.ll -debug  2>$LLVM_BIN/$i.out
   grep "Completed generating HyperOps" $LLVM_BIN/$i.out | wc -l
   
done

# You can access them using echo "${arr[0]}", "${arr[1]}" also

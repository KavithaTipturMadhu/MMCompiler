#!/bin/bash



## declare an array variable
declare -a testCases=("startupglowplugs" "startupstarter" "startuppump_original" "startupoil" "runningregulation_dl" "runningregulation_nh" "runningregulation_nl" "fib")

## now loop through the above array
for i in "${testCases[@]}"
do
   echo "###########Running test case -" $i"###############"
   # or do whatever with individual element of the array
   ./clang -emit-llvm -target redefine /home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/ECU_KERNELS/Compilation_Error/$i.c  -S -o /home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/ECU_KERNELS/Compilation_Error/$i.ll
  ./opt -mem2reg  -load ../lib/HyperOpCreationPass.so -HyperOpCreationPass /home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/ECU_KERNELS/Compilation_Error/$i.ll -S -o /home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/ECU_KERNELS/Compilation_Error/$i.hop.ll -debug  2>out
   grep "Completed generating HyperOps" out | wc -l
   
done

# You can access them using echo "${arr[0]}", "${arr[1]}" also

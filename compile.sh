Release+Asserts/bin/clang -target redefine -emit-llvm  /home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/engineController.c -S -o /home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/engineController.ll

Release+Asserts/bin/opt --load ../lib/HyperOpCreationPass.so --HyperOpCreationPass /home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/engineController.ll -S -o /home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/engineControllerhop.ll -debug 2>out 

Release+Asserts/bin/llc --mtriple=redefine /home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/engineControllerhop.ll -o /home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/engineControllerhop.s  -debug 2>out

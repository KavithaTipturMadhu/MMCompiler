#/bin/bash

TOOLS = $(PWD)/..

# Compiler Build Directory
COMPILER_BIN = $(TOOLS)/MM_Compiler_latest/build/Release+Asserts/bin
#COMPILER_CLANG = $(TOOLS)/Compiler/build/Release+Asserts/bin
ASSEMBLER_DIR = $(TOOLS)/REDEFINE_1CR/Assembler

# Generates the LLVM-IR
CLANG = $(COMPILER_BIN)/clang
CLANG_OPTIONS = -emit-llvm -target redefine -S

# Generates the HyperOp Interaction Graph
OPT = $(COMPILER_BIN)/opt
OPT_OPTIONS = -debug --load $(COMPILER_BIN)/../lib/HyperOpCreationPass.so --HyperOpCreationPass -S

# Spits the Assembly Code
LLC = $(COMPILER_BIN)/llc
LLC_OPTIONS = --mattr=+has4ce --mtriple redefine -debug
#LLC_OPTIONS = --march=redefine --mcpu=REDEFINE --mattr=+has2rows,+has2columns,+has4ce,+wcet 
#LLC_OPTIONS = --mtriple redefine -debug # for single pHyperOp

OUT_CLANG = $(basename $(TARGET)).ll
OUT_OPT 	= $(basename $(TARGET))_HyOpPass.ll
OUT_LLC 	= $(basename $(TARGET)).as
OUT_REF 	= $(basename $(TARGET)).ref

.PHONY : ref

as : clang opt llc

ref : as 
	cd $(ASSEMBLER_DIR) && ./Assembler.py $(TOOLS)/ECU_KERNELS/$(OUT_LLC)

clang :
	$(CLANG) $(CLANG_OPTIONS) $(TARGET) -o $(OUT_CLANG)

opt :
	$(OPT) $(OPT_OPTIONS) $(OUT_CLANG) -o $(OUT_OPT) 2> $(basename $(TARGET))_out_hop

llc :
	$(LLC) $(LLC_OPTIONS) $(OUT_OPT) -o $(OUT_LLC) 2> $(basename $(TARGET))_out_llc

clean :
		cd $(dir $(TARGET)) && rm -f *.as *.ll *out_*

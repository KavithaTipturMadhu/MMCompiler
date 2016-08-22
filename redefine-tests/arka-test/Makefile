#/bin/bash

COMPILER = $(PWD)/../..

# Compiler Build Directory
COMPILER_BIN = $(COMPILER)/build/Release+Asserts/bin

# Generates the LLVM-IR
CLANG = $(COMPILER_BIN)/clang
CLANG_OPTIONS = -emit-llvm -target redefine -S

# Generates the HyperOp Interaction Graph
OPT = $(COMPILER_BIN)/opt
OPT_OPTIONS = -debug --load $(COMPILER_BIN)/../lib/HyperOpCreationPass.so --HyperOpCreationPass -S

# Spits the Assembly Code
LLC = $(COMPILER_BIN)/llc
#LLC_OPTIONS = --mattr=+has1ce,+has3rows,+has3columns --mtriple redefine -debug 
LLC_OPTIONS = --mtriple redefine -debug # for single pHyperOp

OUT_CLANG = $(basename $(TARGET)).ll
OUT_OPT 	= $(basename $(TARGET))hop.ll
OUT_LLC 	= $(basename $(TARGET)).as

.PHONY : llc

clang :
	$(CLANG) $(CLANG_OPTIONS) $(TARGET) -o $(OUT_CLANG)

opt : clang
	$(OPT) $(OPT_OPTIONS) $(OUT_CLANG) -o $(OUT_OPT) 2>out_hop

llc : opt
	$(LLC) $(LLC_OPTIONS) $(OUT_OPT) -o $(OUT_LLC) 2>out_llc

clean :
		rm -rf out_*
		cd $(dir $(TARGET)) && rm -f *.as *.ll

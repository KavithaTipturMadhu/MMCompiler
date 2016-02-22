##===- lib/Target/REDEFINE/Makefile -------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMREDEFINECodeGen
TARGET = REDEFINE

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = REDEFINEGenRegisterInfo.inc \
		REDEFINEGenAsmWriter.inc \
		REDEFINEGenAsmMatcher.inc \
		REDEFINEGenCodeEmitter.inc \
		REDEFINEGenInstrInfo.inc \
		REDEFINEGenDAGISel.inc \
		REDEFINEGenSubtargetInfo.inc \
		REDEFINEGenCallingConv.inc \
		REDEFINEGenMCCodeEmitter.inc

DIRS = InstPrinter AsmParser TargetInfo MCTargetDesc

include $(LEVEL)/Makefile.common


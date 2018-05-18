//===-- REDEFINEAsmPrinter.h - REDEFINE LLVM assembly printer ----*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef REDEFINEASMPRINTER_H
#define REDEFINEASMPRINTER_H

#include "REDEFINETargetMachine.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/Support/Compiler.h"

namespace llvm {
class MCStreamer;
class MachineBasicBlock;
class MachineInstr;
class Module;
class raw_ostream;

class LLVM_LIBRARY_VISIBILITY REDEFINEAsmPrinter: public AsmPrinter {
private:
	const REDEFINESubtarget *Subtarget;
	const char* STATIC_HYPEROP_ANNOTATION = ".STATICINST";
	const char* VALID_ANNOTATION = ".isValid";
	const char* ACTIVE_ANNOTATION = ".isActive";
	const char* INTRINSIC_ANNOTATION = ".isIntrinsic";
	const char* DEPTH_HEG_ANNOTATION = ".depthHEG";
	const char* LAUNCH_CNT_ANNOTATION = ".launchCnt";
	const char* OPERAND_VALIDITY_ANNOTATION = ".OpValidity";
	const char*  OPERAND_BEGIN_ANNOTATION = ".OP_BEGIN";
	const char*  OPERAND_END_ANNOTATION = ".OP_END";
	const char* OP_WAIT_CNT_ANNOTATION = ".OpWaitCnt";
	const char* ISNEXT_HOP_INST_VALID_ANNOTATION = ".isnextHyOpInstValid";
	const char* NEXT_HYPEROP_INST_ANNOTATION = ".nextHypOpInst";
	const char* HYPEROP_INSTANCE_PREFIX =".HyOpInst#";
	list<MCSymbol*> hyperOpLabelsHoistedAsConstants;
	int maxFrameValue = 0;

public:
	REDEFINEAsmPrinter(TargetMachine &TM, MCStreamer &Streamer) :
			AsmPrinter(TM, Streamer) {
		Subtarget = &TM.getSubtarget<REDEFINESubtarget>();
	}

	// Override AsmPrinter.
	virtual const char *getPassName() const LLVM_OVERRIDE {
		return "REDEFINE Assembly Printer";
	}
	virtual void EmitInstruction(const MachineInstr *MI) LLVM_OVERRIDE;
	virtual void EmitMachineConstantPoolValue(MachineConstantPoolValue *MCPV) LLVM_OVERRIDE;
	void printOperand(const MachineInstr *MI, int opNum, raw_ostream &O);
	virtual bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo, unsigned AsmVariant, const char *ExtraCode, raw_ostream &OS) LLVM_OVERRIDE;
	virtual bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNo, unsigned AsmVariant, const char *ExtraCode, raw_ostream &OS) LLVM_OVERRIDE;
	void printMemOperand(const MachineInstr *MI, int opNum, raw_ostream &OS);
	virtual void EmitFunctionBody();
	virtual void EmitFunctionBodyEnd();
	virtual void EmitFunctionEntryLabel();
	virtual bool doInitialization(Module &M);
	virtual bool doFinalization(Module &M);
	virtual void EmitLinkage(unsigned Linkage, MCSymbol *GVSym) const;
    virtual void EmitEndOfAsmFile(Module &);
    virtual void EmitFunctionHeader();
};
} // end namespace llvm

#endif

//==- REDEFINETargetMachine.h - Define TargetMachine for REDEFINE ---*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the REDEFINE specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef REDEFINETARGETMACHINE_H
#define REDEFINETARGETMACHINE_H

#include "REDEFINEFrameLowering.h"
#include "REDEFINEISelLowering.h"
#include "REDEFINEInstrInfo.h"
#include "REDEFINERegisterInfo.h"
#include "REDEFINESubtarget.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetSelectionDAGInfo.h"
#include "llvm/IR/HyperOpInteractionGraph.h"
#include "REDEFINEMCInstrScheduler.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "REDEFINEMCInstrScheduler.h"
using namespace llvm;

namespace llvm {

class REDEFINETargetMachine: public LLVMTargetMachine {
	REDEFINESubtarget Subtarget;
	const DataLayout DL;
	REDEFINEInstrInfo InstrInfo;
	REDEFINETargetLowering TLInfo;
	TargetSelectionDAGInfo TSInfo;
	REDEFINEFrameLowering FrameLowering;

public:
	/* Map of HyperOps and their memory inputs and size in memory frame map */
	map<HyperOp*, map<int, int> > HyperOps;
	map<string, vector<int> > pHyperOpAndNumInputsPerCE;
	//This is required to allocate registers during pHyperOp creation
	FunctionLoweringInfo *FuncInfo;
	REDEFINETargetMachine(const Target &T, StringRef TT, StringRef CPU, StringRef FS, const TargetOptions &Options, Reloc::Model RM, CodeModel::Model CM, CodeGenOpt::Level OL);

	// Override TargetMachine.
	virtual const TargetFrameLowering *getFrameLowering() const LLVM_OVERRIDE {
		return &FrameLowering;
	}

	virtual const REDEFINEInstrInfo *getInstrInfo() const LLVM_OVERRIDE {
		return &InstrInfo;
	}

	virtual const InstrItineraryData *getInstrItineraryData() const
	{  return &Subtarget.getInstrItineraryData(); }

	virtual const REDEFINESubtarget *getSubtargetImpl() const LLVM_OVERRIDE {
		return &Subtarget;
	}
	virtual const DataLayout *getDataLayout() const LLVM_OVERRIDE {
		return &DL;
	}
	virtual const REDEFINERegisterInfo *getRegisterInfo() const LLVM_OVERRIDE {
		return &InstrInfo.getRegisterInfo();
	}
	virtual const REDEFINETargetLowering *getTargetLowering() const LLVM_OVERRIDE {
		return &TLInfo;
	}
	virtual const TargetSelectionDAGInfo *getSelectionDAGInfo() const LLVM_OVERRIDE {
		return &TSInfo;
	}

	// Override LLVMTargetMachine
	virtual TargetPassConfig *createPassConfig(PassManagerBase &PM) LLVM_OVERRIDE;
};

} // end namespace llvm

#endif

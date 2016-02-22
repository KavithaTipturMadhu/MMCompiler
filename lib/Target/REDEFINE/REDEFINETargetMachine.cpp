//===-- REDEFINETargetMachine.cpp - Define TargetMachine for REDEFINE -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "REDEFINETargetMachine.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/Passes.h"

#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

extern "C" void LLVMInitializeREDEFINETarget() {
	// Register the target.
	RegisterTargetMachine<REDEFINETargetMachine> X(TheREDEFINETarget);
}

static ScheduleDAGInstrs *createREDEFINEMachineScheduler(MachineSchedContext *C) {
	return new REDEFINEMCInstrScheduler(C, new REDEFINEILPScheduler(true));
}

static MachineSchedRegistry SchedCustomRegistry("redefine", "Run REDEFINE's custom scheduler", createREDEFINEMachineScheduler);

//static ScheduleDAGInstrs *createREDEFINEMachineSched(MachineSchedContext *C) {
////  return new REDEFINEMachineScheduler(C, MachineSchedRegistry);
//}
REDEFINETargetMachine::REDEFINETargetMachine(const Target &T, StringRef TT, StringRef CPU, StringRef FS, const TargetOptions &Options, Reloc::Model RM, CodeModel::Model CM, CodeGenOpt::Level OL) :
		LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL), Subtarget(TT, CPU, FS),
		// Make sure that global data has at least 16 bits of alignment by default,
		// so that we can refer to it using LARL.  We don't have any special
		// requirements for stack variables though.

		//Quick doc on this incomprehensible garbage below ('-' are delimiters)
		//e -- little endian
		//p:64:64:64 -- pointers are 32bits abi alignment is 32bits preferred alignment is also 32 bits
		//i1:8:16 -- int1 are abi 8 bit aligned and preferred 16
		//i8,u16,i32,f32 -- same as above omitting preferred as needed
		//a0:8:16 -- aggregate type of 0bit? should be 8 bit by api and 16 by pref
		//n32 -- native integer width is 32bits
		DL("e-p:32:32:32-i1:8:16-i8:8:16-i16:16-i32:32"), InstrInfo(*this), TLInfo(*this), TSInfo(*this), FrameLowering(*this, Subtarget) {
}

namespace {
/// REDEFINE Code Generator Pass Configuration Options.
class REDEFINEPassConfig: public TargetPassConfig {
public:
	REDEFINEPassConfig(REDEFINETargetMachine *TM, PassManagerBase &PM) :
			TargetPassConfig(TM, PM) {
		enablePass(&MachineSchedulerID);
		enablePass(&ExpandPostRAPseudosID);
		disablePass(&DeadMachineInstructionElimID);
		disablePass(&StackSlotColoringID);
		disablePass(&PrologEpilogCodeInserterID);
		disablePass(&PostRAMachineLICMID);
		disablePass(&PostRASchedulerID);
		disablePass(&BranchFolderPassID);
		disablePass(&TailDuplicateID);
		disablePass(&MachineCopyPropagationID);
		disablePass(&MachineBlockPlacementID);
		disablePass(&MachineBlockPlacementStatsID);
		disablePass(&MachineCSEID);
		disablePass(&DeadMachineInstructionElimID);
		MachineSchedRegistry::setDefault(createREDEFINEMachineScheduler);
	}

	REDEFINETargetMachine &getREDEFINETargetMachine() const {
		return getTM<REDEFINETargetMachine>();
	}

	virtual bool addInstSelector();

};
} // end anonymous namespace

bool REDEFINEPassConfig::addInstSelector() {
	addPass(createREDEFINEISelDag(getREDEFINETargetMachine(), getOptLevel()));
	return false;
}

TargetPassConfig *REDEFINETargetMachine::createPassConfig(PassManagerBase &PM) {
	return new REDEFINEPassConfig(this, PM);
}

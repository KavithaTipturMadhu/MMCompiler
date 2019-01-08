//===-- REDEFINERegisterInfo.cpp - REDEFINE register information ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "REDEFINERegisterInfo.h"
#include "REDEFINETargetMachine.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/DebugInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#define GET_REGINFO_TARGET_DESC
#include "REDEFINEGenRegisterInfo.inc"

using namespace llvm;

REDEFINERegisterInfo::REDEFINERegisterInfo(REDEFINETargetMachine &tm, const REDEFINEInstrInfo &tii) :
		REDEFINEGenRegisterInfo(REDEFINE::ra), TM(tm), TII(tii) {
}

const uint16_t*
REDEFINERegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
	//TODO This ain't right
    return CSR_REDEFINE32_SaveList;
}

const uint32_t*
REDEFINERegisterInfo::getCallPreservedMask(CallingConv::ID) const {
	return 0;
}

BitVector REDEFINERegisterInfo::getReservedRegs(const MachineFunction &MF) const {
	BitVector Reserved(getNumRegs());
	Reserved.set(REDEFINE::zero);
	Reserved.set(REDEFINE::t5);
	Reserved.set(REDEFINE::t4);
	Reserved.set(REDEFINE::t3);
	Reserved.set(REDEFINE::t2);
	Reserved.set(REDEFINE::t1);
	Reserved.set(REDEFINE::ft0);
	return Reserved;
}

void REDEFINERegisterInfo::eliminateFI(MachineBasicBlock::iterator II, unsigned OpNo, int FrameIndex, uint64_t StackSize, int64_t SPOffset) const {
//TODO
}

void REDEFINERegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj, unsigned FIOperandNum, RegScavenger *RS) const {
	//TODO
}

unsigned REDEFINERegisterInfo::getFrameRegister(const MachineFunction &MF) const {
	return 0;
}

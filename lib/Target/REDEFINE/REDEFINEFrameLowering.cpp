//===-- REDEFINEFrameLowering.cpp - Frame lowering for REDEFINE -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "REDEFINEFrameLowering.h"
#include "REDEFINEInstrBuilder.h"
#include "REDEFINEInstrInfo.h"
#include "REDEFINETargetMachine.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/Function.h"

using namespace llvm;

REDEFINEFrameLowering::REDEFINEFrameLowering(const REDEFINETargetMachine &tm, const REDEFINESubtarget &sti) :
		TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 16, 0), TM(tm), STI(sti) {
}
/*   REDEFINE doesn't have stack frames, using this nevertheless because of the way instruction selection is performed.
 *
 */

// hasFP - Return true if the specified function should have a dedicated frame
// pointer register.  This is true if the function has variable sized allocas or
// if frame pointer elimination is disabled.
bool REDEFINEFrameLowering::hasFP(const MachineFunction &MF) const {
	return false;
}

unsigned REDEFINEFrameLowering::ehDataReg(unsigned I) const {
	static const unsigned EhDataReg[] = { };

	return EhDataReg[I];
}

void REDEFINEFrameLowering::emitPrologue(MachineFunction &MF) const {
	MachineBasicBlock &MBB = MF.front();
	MachineFrameInfo *MFI = MF.getFrameInfo();
//	BuildMI(MBB, MBB.begin(),  MBBI->getDebugLoc(), TII.get(REDEFINE::ADDI)).
}

void REDEFINEFrameLowering::emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const {
//	 const REDEFINEInstrInfo &TII =
//	    *static_cast<const REDEFINEInstrInfo*>(MF.getTarget().getInstrInfo());
//	if (MF.back().getNumber() == MBB.getNumber()) {
//		const Function* F = MF.getFunction();
//		HyperOp* hyperOp = (const_cast<HyperOpInteractionGraph*>(((REDEFINETargetMachine&) TM).HIG))->getHyperOp(*F);
//		map<HyperOpEdge*, HyperOp*> childMap = hyperOp->ChildMap;
//		for (map<HyperOpEdge*, HyperOp*>::iterator childItr = childMap.begin(); childItr != childMap.end(); childItr++) {
//			HyperOpEdge* edge = childItr.first;
//
//			if(edge->Type==HyperOpEdge::DATA){
//				string name = ((DataDependenceEdge*)edge)->Data->variable;
//				for(list<MachineInstr>::iterator itr = MBB.begin();itr!=MBB.end();itr++){
//					if((*itr).getOperand(0).isDef()&&)
//				}
//			BuildMI(MBB, MBB.end(), MBB.end(), TII.get(REDEFINE::WRITECM)).
//			}else if(edge->Type == HyperOpEdge::CONTROL){
//				BuildMI(MBB, MBB.end(), MBB.end(), TII.get(REDEFINE::WRITECMP)).
//			}
//		}
//	}

}

bool REDEFINEFrameLowering::spillCalleeSavedRegisters(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, const std::vector<CalleeSavedInfo> &CSI, const TargetRegisterInfo *TRI) const {
	return true;
}

bool REDEFINEFrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
	return false;
}

// Eliminate ADJCALLSTACKDOWN, ADJCALLSTACKUP pseudo instructions
void REDEFINEFrameLowering::eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const {
}

void REDEFINEFrameLowering::processFunctionBeforeCalleeSavedScan(MachineFunction &MF, RegScavenger *RS) const {

}

//===-- REDEFINEInstrInfo.cpp - REDEFINE instruction information ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the REDEFINE implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "REDEFINEInstrInfo.h"
#include "REDEFINEInstrBuilder.h"
#include "REDEFINETargetMachine.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/MC/MCContext.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/IR/REDEFINEUtils.h"

#define GET_INSTRINFO_CTOR
#define GET_INSTRMAP_INFO
#include "REDEFINEGenInstrInfo.inc"

using namespace llvm;

REDEFINEInstrInfo::REDEFINEInstrInfo(REDEFINETargetMachine &tm) :
		REDEFINEGenInstrInfo(), RI(tm, *this), TM(tm) {
}

unsigned REDEFINEInstrInfo::GetInstSizeInBytes(MachineInstr *I) const {
	//Since we don't have variable length instructions this just looks at the subtarget
//  const REDEFINESubtarget &STI = TM.getSubtarget<REDEFINESubtarget>();
//  return (STI.isRV64() || STI.isRV32()) ? 4 : 4;
	return 4;
}

bool REDEFINEInstrInfo::AnalyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB, MachineBasicBlock *&FBB, SmallVectorImpl<MachineOperand> &Cond, bool AllowModify) const {
	// Most of the code and comments here are boilerplate.

	// Start from the bottom of the block and work up, examining the
	// terminator instructions.
	MachineBasicBlock::iterator I = MBB.end();
	while (I != MBB.begin()) {
		--I;
		if (I->isDebugValue())
			continue;

		// Working from the bottom, when we see a non-terminator instruction, we're
		// done.
		if (!isUnpredicatedTerminator(I))
			break;

		// A terminator that isn't a branch can't easily be handled by this
		// analysis.
		SmallVector<MachineOperand, 4> ThisCond;
		ThisCond.push_back(MachineOperand::CreateImm(0));
		const MachineOperand *ThisTarget;
		if (!isBranch(I, ThisCond, ThisTarget))
			return true;

		// Can't handle indirect branches.
		if (!ThisTarget->isMBB())
			return true;

		if (ThisCond[0].getImm() == REDEFINE::CCMASK_ANY) {
			// Handle unconditional branches.
			if (!AllowModify) {
				TBB = ThisTarget->getMBB();
				continue;
			}

			// If the block has any instructions after a JMP, delete them.
			while (llvm::next(I) != MBB.end())
				llvm::next(I)->eraseFromParent();

			Cond.clear();
			FBB = 0;

			// Delete the JMP if it's equivalent to a fall-through.
			/*We can't do this now because the BBs can still be rearranged
			 if (MBB.isLayoutSuccessor(ThisTarget->getMBB())) {
			 TBB = 0;
			 I->eraseFromParent();
			 I = MBB.end();
			 continue;
			 }
			 */

			// TBB is used to indicate the unconditinal destination.
			TBB = ThisTarget->getMBB();
			continue;
		}

		// Working from the bottom, handle the first conditional branch.
		if (Cond.empty()) {
			// FIXME: add X86-style branch swap
			FBB = TBB;
			TBB = ThisTarget->getMBB();
			Cond.push_back(MachineOperand::CreateImm(ThisCond[0].getImm()));
			//push remaining operands
			for (unsigned int i = 0; i < (I->getNumExplicitOperands()); i++)
				Cond.push_back(I->getOperand(i));

			continue;
		}

		// Handle subsequent conditional branches.
		assert(Cond.size() <= 4);
		assert(TBB);

		// Only handle the case where all conditional branches branch to the same
		// destination.
		if (TBB != ThisTarget->getMBB())
			return true;

		// If the conditions are the same, we can leave them alone.
		unsigned OldCond = Cond[0].getImm();
		if (OldCond == ThisCond[0].getImm())
			continue;

		// FIXME: Try combining conditions like X86 does.
	}

	return false;
}

unsigned REDEFINEInstrInfo::RemoveBranch(MachineBasicBlock &MBB) const {
	// Most of the code and comments here are boilerplate.
	MachineBasicBlock::iterator I = MBB.end();
	unsigned Count = 0;

	while (I != MBB.begin()) {
		--I;
		if (I->isDebugValue())
			continue;
		SmallVector<MachineOperand, 4> Cond;
		Cond.push_back(MachineOperand::CreateImm(0));
		const MachineOperand *Target;
		if (!isBranch(I, Cond, Target))
			break;
		if (!Target->isMBB())
			break;
		// Remove the branch.
		I->eraseFromParent();
		I = MBB.end();
		++Count;
	}

	return Count;
}

unsigned REDEFINEInstrInfo::InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB, const SmallVectorImpl<MachineOperand> &Cond, DebugLoc DL) const {
	//REDEFINE doesn't have any two way branches, can only have FBB if it is a fall through
	if (FBB && !MBB.isLayoutSuccessor(FBB)) {
		//Need to build two branches then
		//one to branch to TBB on Cond
		//and a second one immediately after to unconditionally jump to FBB
		unsigned count = InsertBranchAtInst(MBB, MBB.end(), TBB, Cond, DL);
		BuildMI(&MBB, DL, get(REDEFINE::BEQ)).addImm(0).addReg(Cond[2].getReg()).addReg(Cond[2].getReg()).addMBB(FBB);
		count++;
		return count;
	}
	//This function inserts the branch at the end of the MBB
	return InsertBranchAtInst(MBB, MBB.end(), TBB, Cond, DL);
}

unsigned REDEFINEInstrInfo::InsertConstBranchAtInst(MachineBasicBlock &MBB, MachineInstr *I, int64_t offset, const SmallVectorImpl<MachineOperand> &Cond, DebugLoc DL) const {
	// Shouldn't be a fall through.
	assert(&MBB && "InsertBranch must not be told to insert a fallthrough");
	assert(Cond.size() <= 4 && "REDEFINE branch conditions have less than four components!");

	if (Cond.empty() || Cond[0].getImm() == REDEFINE::CCMASK_ANY) {
		// Unconditional branch
		BuildMI(MBB, I, DL, get(REDEFINE::JAL)).addImm(offset);
		return 1;
	}

	// Conditional branch.
	unsigned Count = 0;
	unsigned CC = Cond[0].getImm();
	switch (CC) {
	case REDEFINE::CCMASK_CMP_EQ:
		BuildMI(MBB, I, DL, get(REDEFINE::BEQ)).addImm(offset).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case REDEFINE::CCMASK_CMP_GT:
		BuildMI(MBB, I, DL, get(REDEFINE::BGT)).addImm(offset).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case REDEFINE::CCMASK_CMP_NE:
		BuildMI(MBB, I, DL, get(REDEFINE::BNE)).addImm(offset).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case REDEFINE::CCMASK_CMP_LT:
		BuildMI(MBB, I, DL, get(REDEFINE::BLT)).addImm(offset).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case REDEFINE::CCMASK_CMP_LE:
		BuildMI(MBB, I, DL, get(REDEFINE::BLE)).addImm(offset).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case (REDEFINE::CCMASK_CMP_LT | REDEFINE::CCMASK_CMP_UO):
		BuildMI(MBB, I, DL, get(REDEFINE::BLTU)).addImm(offset).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case REDEFINE::CCMASK_CMP_GE:
		BuildMI(MBB, I, DL, get(REDEFINE::BGE)).addImm(offset).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case (REDEFINE::CCMASK_CMP_GE | REDEFINE::CCMASK_CMP_UO):
		BuildMI(MBB, I, DL, get(REDEFINE::BGEU)).addImm(offset).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case (REDEFINE::CCMASK_CMP_GT | REDEFINE::CCMASK_CMP_UO):
		BuildMI(MBB, I, DL, get(REDEFINE::BGTU)).addImm(offset).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case (REDEFINE::CCMASK_CMP_LE | REDEFINE::CCMASK_CMP_UO):
		BuildMI(MBB, I, DL, get(REDEFINE::BLEU)).addImm(offset).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	default:
		llvm_unreachable("Invalid branch condition code!");
	}
	++Count;
	return Count;
}

unsigned REDEFINEInstrInfo::InsertBranchAtInst(MachineBasicBlock &MBB, MachineInstr *I, MachineBasicBlock *TBB, const SmallVectorImpl<MachineOperand> &Cond, DebugLoc DL) const {
	// Shouldn't be a fall through.
	assert(TBB && "InsertBranch must not be told to insert a fallthrough");
	assert(Cond.size() <= 4 && "REDEFINE branch conditions have less than four components!");
	if (Cond.empty() || Cond[0].getImm() == REDEFINE::CCMASK_ANY) {
		// Unconditional branch
		BuildMI(MBB, I, DL, get(REDEFINE::JAL)).addMBB(TBB);
		return 1;
	}
	// Conditional branch.
	unsigned Count = 0;
	unsigned CC = Cond[0].getImm();
	errs()<<"whats cc ?"<<CC<<", ";
	switch (CC) {
	case REDEFINE::CCMASK_CMP_EQ:
		BuildMI(MBB, I, DL, get(REDEFINE::BEQ)).addMBB(TBB).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case REDEFINE::CCMASK_CMP_GT:
		BuildMI(MBB, I, DL, get(REDEFINE::BGT)).addMBB(TBB).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case REDEFINE::CCMASK_CMP_NE:
		BuildMI(MBB, I, DL, get(REDEFINE::BNE)).addMBB(TBB).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case REDEFINE::CCMASK_CMP_LT:
		BuildMI(MBB, I, DL, get(REDEFINE::BLT)).addMBB(TBB).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case REDEFINE::CCMASK_CMP_LE:
		BuildMI(MBB, I, DL, get(REDEFINE::BLE)).addMBB(TBB).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case (REDEFINE::CCMASK_CMP_LT | REDEFINE::CCMASK_CMP_UO):
		BuildMI(MBB, I, DL, get(REDEFINE::BLTU)).addMBB(TBB).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case REDEFINE::CCMASK_CMP_GE:
		BuildMI(MBB, I, DL, get(REDEFINE::BGE)).addMBB(TBB).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case (REDEFINE::CCMASK_CMP_GE | REDEFINE::CCMASK_CMP_UO):
		BuildMI(MBB, I, DL, get(REDEFINE::BGEU)).addMBB(TBB).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case (REDEFINE::CCMASK_CMP_GT | REDEFINE::CCMASK_CMP_UO):
		BuildMI(MBB, I, DL, get(REDEFINE::BGTU)).addMBB(TBB).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	case (REDEFINE::CCMASK_CMP_LE | REDEFINE::CCMASK_CMP_UO):
		BuildMI(MBB, I, DL, get(REDEFINE::BLEU)).addMBB(TBB).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
		break;
	default:
		llvm_unreachable("Invalid branch condition code!");
	}
	++Count;

	return Count;
}

void REDEFINEInstrInfo::copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI, DebugLoc DL, unsigned DestReg, unsigned SrcReg, bool KillSrc) const {

	bool isBundledWithPred = MBBI->isBundledWithPred();
	bool isBundledWithSucc = MBBI->isBundledWithSucc();
	MachineInstr* Pred;
	if (isBundledWithPred) {
		Pred = MBBI;
		Pred = Pred->getPrevNode();
	}

	unsigned Opcode;
	const REDEFINESubtarget &STI = TM.getSubtarget<REDEFINESubtarget>();
	MachineInstr* createdMachineInstr;
	//when we are copying a phys reg we want the bits for fp
	if (REDEFINE::GR32BitRegClass.contains(DestReg, SrcReg) || REDEFINE::FP32BitRegClass.contains(DestReg, SrcReg)) {
		MachineInstrBuilder builder = BuildMI(MBB, MBBI, DL, get(REDEFINE::ADDI), DestReg).addReg(SrcReg, getKillRegState(KillSrc)).addImm(0);
		createdMachineInstr = builder.operator ->();
	} else if (REDEFINE::GR32BitRegClass.contains(DestReg) && REDEFINE::FP32BitRegClass.contains(SrcReg)) {
		MachineInstrBuilder builder = BuildMI(MBB, MBBI, DL, get(REDEFINE::FMV_X_S), DestReg).addReg(SrcReg, getKillRegState(KillSrc));
		createdMachineInstr = builder.operator ->();
	} else if (REDEFINE::GR32BitRegClass.contains(SrcReg) && REDEFINE::FP32BitRegClass.contains(DestReg)) {
		MachineInstrBuilder builder = BuildMI(MBB, MBBI, DL, get(REDEFINE::FMV_S_X), DestReg).addReg(SrcReg, getKillRegState(KillSrc));
		createdMachineInstr = builder.operator ->();
	} else {
		llvm_unreachable("Impossible reg-to-reg copy");
	}

	MBBI->clearFlag(MachineInstr::BundledSucc);
	MBBI->clearFlag(MachineInstr::BundledPred);
	if (isBundledWithPred) {
		Pred->clearFlag(MachineInstr::BundledSucc);
		createdMachineInstr->bundleWithPred();
	}
	if (isBundledWithSucc) {
		createdMachineInstr->bundleWithSucc();
	}

}

void
REDEFINEInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
				      MachineBasicBlock::iterator MBBI,
				      unsigned SrcReg, bool isKill,
				      int FrameIdx,
				      const TargetRegisterClass *RC,
				      const TargetRegisterInfo *TRI) const {
  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();


  // Callers may expect a single instruction, so keep 128-bit moves
  // together for now and lower them after register allocation.
  unsigned LoadOpcode, StoreOpcode;
  getLoadStoreOpcodes(RC, LoadOpcode, StoreOpcode);
  MBB.dump();

  MachineInstrBuilder sw =  BuildMI(MBB, MBBI, DL, get(StoreOpcode));
  addFrameReference(sw.addReg(SrcReg, getKillRegState(isKill)), FrameIdx);
  if(!sw->isBundledWithPred()){
	  sw->bundleWithPred();

  }
  sw->dump();
  errs()<<"Inside storeRegToStackSlot\n";
  MBB.dump();
  errs()<<MBBI->isInsideBundle()<<MBBI->isBundledWithPred()<<MBBI->isBundledWithSucc()<<MBBI->isBundled()<<"\n";


}

void
REDEFINEInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
				       MachineBasicBlock::iterator MBBI,
				       unsigned DestReg, int FrameIdx,
				       const TargetRegisterClass *RC,
				       const TargetRegisterInfo *TRI) const {
  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  // Callers may expect a single instruction, so keep 128-bit moves
  // together for now and lower them after register allocation.
  unsigned LoadOpcode, StoreOpcode;
  getLoadStoreOpcodes(RC, LoadOpcode, StoreOpcode);
  MachineInstrBuilder lw = BuildMI(MBB, MBBI, DL, get(LoadOpcode), DestReg);
  addFrameReference(lw, FrameIdx);
  if(!lw->isBundledWithSucc()){
	  lw->bundleWithSucc();
  }

}

bool REDEFINEInstrInfo::expandPostRAPseudo(MachineBasicBlock::iterator MI) const {

	if (MI->getOpcode() == REDEFINE::ADDI && MI->getOperand(2).getType() == MachineOperand::MO_FrameIndex) {
		const MachineOperand &MO = MI->getOperand(2);
		const MachineFrameInfo* frameInfo = MO.getParent()->getParent()->getParent()->getFrameInfo();
		unsigned currentObjectOffset = 0;
		if (MO.getType() == MachineOperand::MO_FrameIndex) {
			for (int i = 0; i < MO.getIndex(); i++) {
				currentObjectOffset += REDEFINEUtils::getSizeOfType(frameInfo->getObjectAllocation(i)->getType());
			}
		}
		if (!isInt<12>(currentObjectOffset)) {
			MI->dump();
			MachineInstrBuilder movImm = BuildMI(*(MI->getParent()), MI, MI->getDebugLoc(), get(REDEFINE::MOVIMM));
			movImm.addReg(REDEFINE::t0, RegState::Define);
			movImm.addImm(currentObjectOffset);
			MachineInstrBuilder add = BuildMI(*(MI->getParent()), MI, MI->getDebugLoc(), get(REDEFINE::ADD)).addReg(MI->getOperand(0).getReg()).addReg(MI->getOperand(1).getReg()).addReg(REDEFINE::t0);
			if (MI->isBundled()) {
				MI->eraseFromBundle();
			} else {
				MI->eraseFromParent();
			}
			movImm->bundleWithSucc();
			add->bundleWithSucc();
		}
	}
//	//TODO Hack for immediates that don't fit in 12 bit addi operand field
//	if (MI->getOpcode() == REDEFINE::ADDI && MI->getOperand(1).getReg() == REDEFINE::zero) {
//		if (MI->getOperand(2).isImm() && MI->getOperand(2).getImm() != 0 && Log2_32_Ceil((uint32_t) MI->getOperand(2).getImm()) > 10) {
//			//Since immediate value cannot spill 11 bits, we need to expand it to lui and add instructions
//			MachineBasicBlock::instr_iterator Pred, Succ;
//			//TODO We know that an immediate value cannot exceed 32 bit value anyway, so casting to 32 bit is expected to be safe
//			int32_t immediateValue = ((int32_t) MI->getOperand(2).getImm());
//			bool isMIBundledWithPred = MI->isBundledWithPred();
//			bool isMIBundledWithSucc = MI->isBundledWithSucc();
//			if (isMIBundledWithPred) {
//				Pred = MI.getInstrIterator();
//				--Pred;
//			}
//			if (isMIBundledWithSucc) {
//				Succ = MI.getInstrIterator();
//				++Succ;
//			}
//
//			unsigned addiRegister = MI->getOperand(0).getReg();
//
//			MachineInstrBuilder luiForTopBits = BuildMI(*(MI->getParent()), MI, MI->getDebugLoc(), get(REDEFINE::LUI)).addReg(addiRegister, RegState::Define).addImm((immediateValue & 0xfffff000) >> 12);
//			MachineInstrBuilder luiForBottomBits = BuildMI(*(MI->getParent()), MI, MI->getDebugLoc(), get(REDEFINE::LUI)).addReg(REDEFINE::a5, RegState::Define).addImm(immediateValue & 0xfff);
//			MachineInstrBuilder srliForBottomBits = BuildMI(*(MI->getParent()), MI, MI->getDebugLoc(), get(REDEFINE::SRLI)).addReg(REDEFINE::a5, RegState::Define).addReg(REDEFINE::a5, RegState::InternalRead).addImm(12);
//			MachineInstrBuilder add = BuildMI(*(MI->getParent()), MI, MI->getDebugLoc(), get(REDEFINE::ADD)).addReg(addiRegister).addReg(addiRegister).addReg(REDEFINE::a5);
//
//			if (MI->isBundled()) {
//				MI->eraseFromBundle();
//			} else {
//				MI->eraseFromParent();
//			}
//			luiForTopBits->bundleWithSucc();
//			luiForBottomBits->bundleWithSucc();
//			srliForBottomBits->bundleWithSucc();
//
//			if (isMIBundledWithPred) {
//				//TODO Couldn't use unbundlefromsucc and unbundlefrompredecessor directly here
//				Pred->clearFlag(MachineInstr::BundledSucc);
//				luiForTopBits->bundleWithPred();
//			}
//			if (isMIBundledWithSucc) {
//				Succ->clearFlag(MachineInstr::BundledPred);
//				add->bundleWithSucc();
//			}
//			return true;
//		} else if (MI->getOperand(2).isGlobal()) {
//			//This is only in case of getting the lower bits of addi
//			const GlobalValue * gv = MI->getOperand(2).getGlobal();
//			const Module* parentModule = MI->getParent()->getParent()->getFunction()->getParent();
//			unsigned maxGlobalSize = 0;
//			if (!parentModule->getGlobalList().empty()) {
//				for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
//					const GlobalVariable *globalVar = &*globalArgItr;
//					if (globalVar->getName().compare(gv->getName()) == 0) {
//						break;
//					}
//					maxGlobalSize += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
//				}
//			}
//
//			MachineBasicBlock::instr_iterator Pred, Succ;
//			bool isMIBundledWithPred = MI->isBundledWithPred();
//			bool isMIBundledWithSucc = MI->isBundledWithSucc();
//			if (isMIBundledWithPred) {
//				Pred = MI.getInstrIterator();
//				--Pred;
//			}
//			if (isMIBundledWithSucc) {
//				Succ = MI.getInstrIterator();
//				++Succ;
//			}
//
//			unsigned addiRegister = MI->getOperand(0).getReg();
//
//			MachineInstrBuilder lui = BuildMI(*(MI->getParent()), MI, MI->getDebugLoc(), get(REDEFINE::LUI));
//			lui.addReg(addiRegister, RegState::Define);
//			string lowGlobalAddrString = string("%lo(").append("\"ga#").append(itostr(maxGlobalSize)).append("\")");
//			MCSymbol* loSymbol = lui.operator ->()->getParent()->getParent()->getContext().GetOrCreateSymbol(StringRef(lowGlobalAddrString));
//			lui.addSym(loSymbol);
//
//			MachineInstrBuilder shiftInstr = BuildMI(*(MI->getParent()), MI, MI->getDebugLoc(), get(REDEFINE::SRLI)).addReg(addiRegister).addReg(addiRegister).addImm(12);
//			if (MI->isBundled()) {
//				MI->eraseFromBundle();
//			} else {
//				MI->eraseFromParent();
//			}
//			lui->bundleWithSucc();
//
//			if (isMIBundledWithPred) {
//				//TODO Couldn't use unbundlefromsucc and unbundlefrompredecessor directly here
//				Pred->clearFlag(MachineInstr::BundledSucc);
//				lui->bundleWithPred();
//			}
//			if (isMIBundledWithSucc) {
//				Succ->clearFlag(MachineInstr::BundledPred);
//				shiftInstr->bundleWithSucc();
//			}
//			return true;
//		}
//	}
	return false;

}

bool REDEFINEInstrInfo::ReverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const {
	assert(Cond.size() <= 4 && "Invalid branch condition!");
//Only need to switch the condition code, not the registers
	switch (Cond[0].getImm()) {
	case REDEFINE::CCMASK_CMP_EQ:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_NE);
		return false;
	case REDEFINE::CCMASK_CMP_NE:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_EQ);
		return false;
	case REDEFINE::CCMASK_CMP_LT:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_GE);
		return false;
	case REDEFINE::CCMASK_CMP_GE:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_LT);
		return false;
	case REDEFINE::CCMASK_CMP_LT | REDEFINE::CCMASK_CMP_UO:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_GE | REDEFINE::CCMASK_CMP_UO);
		return false;
	case REDEFINE::CCMASK_CMP_GE | REDEFINE::CCMASK_CMP_UO:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_LT | REDEFINE::CCMASK_CMP_UO);
		return false;
		//synth
	case REDEFINE::CCMASK_CMP_GT:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_LE);
		return false;
	case REDEFINE::CCMASK_CMP_LE:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_GT);
		return false;
	case REDEFINE::CCMASK_CMP_GT | REDEFINE::CCMASK_CMP_UO:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_LE | REDEFINE::CCMASK_CMP_UO);
		return false;
	case REDEFINE::CCMASK_CMP_LE | REDEFINE::CCMASK_CMP_UO:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_GT | REDEFINE::CCMASK_CMP_UO);
		return false;
	default:
		llvm_unreachable("Invalid branch condition!");
	}
}

bool REDEFINEInstrInfo::isBranch(const MachineInstr *MI, SmallVectorImpl<MachineOperand> &Cond, const MachineOperand *&Target) const {
	switch (MI->getOpcode()) {
	case REDEFINE::JAL:
		Cond[0].setImm(REDEFINE::CCMASK_ANY);
		Target = &MI->getOperand(0);
		return true;
	case REDEFINE::BEQ:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_EQ);
		Target = &MI->getOperand(0);
		return true;
	case REDEFINE::BNE:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_NE);
		Target = &MI->getOperand(0);
		return true;
	case REDEFINE::BLT:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_LT);
		Target = &MI->getOperand(0);
		return true;
	case REDEFINE::BLTU:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_LT | REDEFINE::CCMASK_CMP_UO);
		Target = &MI->getOperand(0);
		return true;
	case REDEFINE::BGE:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_GE);
		Target = &MI->getOperand(0);
		return true;
	case REDEFINE::BGEU:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_GE | REDEFINE::CCMASK_CMP_UO);
		Target = &MI->getOperand(0);
		return true;
////synth
	case REDEFINE::BGT:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_GT);
		Target = &MI->getOperand(0);
		return true;
	case REDEFINE::BGTU:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_GT | REDEFINE::CCMASK_CMP_UO);
		Target = &MI->getOperand(0);
		return true;
	case REDEFINE::BLE:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_LE);
		Target = &MI->getOperand(0);
		return true;
	case REDEFINE::BLEU:
		Cond[0].setImm(REDEFINE::CCMASK_CMP_LE | REDEFINE::CCMASK_CMP_UO);
		Target = &MI->getOperand(0);
		return true;

	default:
		MI->dump();
		assert(!MI->getDesc().isBranch() && "Unknown branch opcode");
		return false;
	}
}

void REDEFINEInstrInfo::getLoadStoreOpcodes(const TargetRegisterClass *RC, unsigned &LoadOpcode, unsigned &StoreOpcode) const {
	const REDEFINESubtarget &STI = TM.getSubtarget<REDEFINESubtarget>();
	if (RC == &REDEFINE::GR32BitRegClass) {
		LoadOpcode = REDEFINE::LW;
		StoreOpcode = REDEFINE::SW;
	} else
		llvm_unreachable("Unsupported regclass to load or store");
}

unsigned REDEFINEInstrInfo::getOpcodeForOffset(unsigned Opcode, int64_t Offset) const {
	int64_t Offset2 = Offset;
	if (isInt<12>(Offset) && isInt<12>(Offset2)) {
		return Opcode;
	}
	if (isInt<20>(Offset) && isInt<20>(Offset2)) {
		return Opcode;
	}
	return 0;
}

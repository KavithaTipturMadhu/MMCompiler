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
		if (I->isDebugValue()) continue;

		// Working from the bottom, when we see a non-terminator instruction, we're
		// done.
		if (!isUnpredicatedTerminator(I)) break;

		// A terminator that isn't a branch can't easily be handled by this
		// analysis.
		SmallVector<MachineOperand, 4> ThisCond;
		ThisCond.push_back(MachineOperand::CreateImm(0));
		const MachineOperand *ThisTarget;
		if (!isBranch(I, ThisCond, ThisTarget)) return true;

		// Can't handle indirect branches.
		if (!ThisTarget->isMBB()) return true;

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
		if (TBB != ThisTarget->getMBB()) return true;

		// If the conditions are the same, we can leave them alone.
		unsigned OldCond = Cond[0].getImm();
		if (OldCond == ThisCond[0].getImm()) continue;

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
		if (I->isDebugValue()) continue;
		SmallVector<MachineOperand, 4> Cond;
		Cond.push_back(MachineOperand::CreateImm(0));
		const MachineOperand *Target;
		if (!isBranch(I, Cond, Target)) break;
		if (!Target->isMBB()) break;
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
		case REDEFINE::CCMASK_CMP_NE:
			BuildMI(MBB, I, DL, get(REDEFINE::BNE)).addImm(offset).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
			break;
		case REDEFINE::CCMASK_CMP_LT:
			BuildMI(MBB, I, DL, get(REDEFINE::BLT)).addImm(offset).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
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
	switch (CC) {
		case REDEFINE::CCMASK_CMP_EQ:
			BuildMI(MBB, I, DL, get(REDEFINE::BEQ)).addMBB(TBB).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
			break;
		case REDEFINE::CCMASK_CMP_NE:
			BuildMI(MBB, I, DL, get(REDEFINE::BNE)).addMBB(TBB).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
			break;
		case REDEFINE::CCMASK_CMP_LT:
			BuildMI(MBB, I, DL, get(REDEFINE::BLT)).addMBB(TBB).addReg(Cond[2].getReg()).addReg(Cond[3].getReg());
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
		default:
			llvm_unreachable("Invalid branch condition code!");
	}
	++Count;

	return Count;
}

void REDEFINEInstrInfo::copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI, DebugLoc DL, unsigned DestReg, unsigned SrcReg, bool KillSrc) const {

	unsigned Opcode;
	const REDEFINESubtarget &STI = TM.getSubtarget<REDEFINESubtarget>();
	//when we are copying a phys reg we want the bits for fp
	if (REDEFINE::GR32BitRegClass.contains(DestReg, SrcReg)) Opcode = REDEFINE::ADDI;
	else
	llvm_unreachable("Impossible reg-to-reg copy");

	BuildMI(MBB, MBBI, DL, get(Opcode), DestReg).addReg(SrcReg, getKillRegState(KillSrc)).addImm(0);
}

bool REDEFINEInstrInfo::expandPostRAPseudo(MachineBasicBlock::iterator MI) const {
	//Hack for immediates that don't fit in 12 bit addi operand field
	if (MI->getOpcode() == REDEFINE::ADDI && MI->getOperand(1).getReg() == REDEFINE::zero&&MI->getOperand(2).isImm()) {
		MachineOperand& immediateOperand = MI->getOperand(2);
		if (ceil(log2(immediateOperand.getImm()))>12) {
			//TODO We know that an immediate value cannot exceed 32 bit value anyway, so casting to 32 bit is expected to be safe
			int32_t immediateValue = ((int32_t) immediateOperand.getImm());

			bool isMIBundledWithSucc = MI->isBundledWithSucc();

			//Split the addi to an srli, lui and addi
			MachineInstrBuilder lui = BuildMI(*(MI->getParent()), MI, MI->getDebugLoc(),  get(REDEFINE::LUI));
			lui.addImm(immediateValue >> 12);
			lui.addReg(MI->getOperand(0).getReg(), RegState::Define);

			MachineInstrBuilder addi = BuildMI(*(MI->getParent()), MI, MI->getDebugLoc(), get(REDEFINE::ADDI));
			addi.addReg(MI->getOperand(0).getReg(), RegState::Kill);
			addi.addReg(REDEFINE::zero, RegState::InternalRead);
			addi.addImm(immediateValue & 0xfff);

			//We know the Machine Instruction is bundled for certain
			MI->eraseFromBundle();

			lui.operator ->()->bundleWithSucc();
			if(isMIBundledWithSucc){
				addi.operator ->()->bundleWithSucc();
			}
			return true;
		}
	}
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
	}
	else
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


//===-- REDEFINEAsmPrinter.cpp - REDEFINE LLVM assembly printer -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Streams REDEFINE assembly language and associated data, in the form of
// MCInsts and MCExprs respectively.
//
//===----------------------------------------------------------------------===//

#include "REDEFINEAsmPrinter.h"
#include "InstPrinter/REDEFINEInstPrinter.h"
#include "REDEFINEConstantPoolValue.h"
#include "REDEFINEMCInstLower.h"
#include "llvm/CodeGen/MachineModuleInfoImpls.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/Mangler.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/MC/MCSymbol.h"

using namespace llvm;

void REDEFINEAsmPrinter::EmitInstruction(const MachineInstr *MI) {
	REDEFINEMCInstLower Lower(Mang, MF->getContext(), *this);
	MCInst LoweredMI;
	Lower.lower(MI, LoweredMI);
	OutStreamer.EmitInstruction(LoweredMI);
}

// Convert a REDEFINE-specific constant pool modifier into the associated
// MCSymbolRefExpr variant kind.
static MCSymbolRefExpr::VariantKind getModifierVariantKind(REDEFINECP::REDEFINECPModifier Modifier) {
	switch (Modifier) {
	case REDEFINECP::NTPOFF:
		return MCSymbolRefExpr::VK_NTPOFF;
	}
	llvm_unreachable("Invalid SystemCPModifier!");
}

void REDEFINEAsmPrinter::EmitMachineConstantPoolValue(MachineConstantPoolValue *MCPV) {
	REDEFINEConstantPoolValue *ZCPV = static_cast<REDEFINEConstantPoolValue*>(MCPV);

	const MCExpr *Expr = MCSymbolRefExpr::Create(Mang->getSymbol(ZCPV->getGlobalValue()), getModifierVariantKind(ZCPV->getModifier()), OutContext);
	uint64_t Size = TM.getDataLayout()->getTypeAllocSize(ZCPV->getType());

	OutStreamer.EmitValue(Expr, Size);
}
void REDEFINEAsmPrinter::EmitFunctionBody() {
	int ceCount = ((REDEFINETargetMachine&) TM).getSubtargetImpl()->getCeCount();
	// Emit target-specific gunk before the function body.
	EmitFunctionBodyStart();

	// Print out code for the function.
	bool HasAnyRealCode = false;
	const MachineInstr *LastMI = 0;
	vector<list<const MachineInstr*> > pHyperOpInstructions(ceCount);
	vector<list<const MachineInstr*> > startOfBBInPHyperOp(ceCount);
	for (MachineFunction::const_iterator I = MF->begin(), E = MF->end(); I != E; ++I) {
		int pHyperOpIndex = -1;
		for (MachineBasicBlock::const_instr_iterator instrItr = I->instr_begin(); instrItr != I->instr_end(); ++instrItr) {
			//First instruction of the pHyperOp is never in a bundle
			if (!instrItr->isInsideBundle()) {
				pHyperOpIndex++;
				startOfBBInPHyperOp[pHyperOpIndex].push_back(instrItr);
			}
			pHyperOpInstructions[pHyperOpIndex].push_back(instrItr);
		}
	}


	int pHyperOpIndex = 0;
	for (vector<list<const MachineInstr*> >::iterator pHyperOpItr = pHyperOpInstructions.begin(); pHyperOpItr != pHyperOpInstructions.end(); pHyperOpItr++, pHyperOpIndex++) {
		OutStreamer.EmitRawText(StringRef("\npHyperOp:\n"));
		for (list<const MachineInstr*>::iterator mcItr = pHyperOpItr->begin(); mcItr != pHyperOpItr->end(); mcItr++) {
			if (startOfBBInPHyperOp[pHyperOpIndex].front() == *mcItr) {
				MCSymbol *label = (*mcItr)->getParent()->getSymbol();
				label->setUndefined();
				OutStreamer.EmitLabel(label);
				startOfBBInPHyperOp[pHyperOpIndex].pop_front();
			}
			EmitInstruction(*mcItr);
		}

	}
}

//void REDEFINEAsmPrinter::EmitFunctionHeader(){
//	EmitFunctionEntryLabel();
//}

void REDEFINEAsmPrinter::printOperand(const MachineInstr *MI, int OpNo, raw_ostream &O) {
	const MachineOperand &MO = MI->getOperand(OpNo);
	//look at target flags to see if we should wrap this operand
	switch (MO.getTargetFlags()) {
	case REDEFINEII::MO_ABS_HI:
		O << "%hi(";
		break;
	case REDEFINEII::MO_ABS_LO:
		O << "%lo(";
		break;
	case REDEFINEII::MO_TPREL_HI:
		O << "%tprel_hi(";
		break;
	case REDEFINEII::MO_TPREL_LO:
		O << "%tprel_lo(";
		break;
	}
	switch (MO.getType()) {
	case MachineOperand::MO_Register:
	case MachineOperand::MO_Immediate: {
		REDEFINEMCInstLower Lower(Mang, MF->getContext(), *this);
		MCOperand MC(Lower.lowerOperand(MI->getOperand(OpNo)));
		REDEFINEInstPrinter::printOperand(MC, O);
		break;
	}
	case MachineOperand::MO_GlobalAddress:
		O << Mang->getSymbol(MO.getGlobal());
		break;
	default:
		llvm_unreachable("<unknown operand type>");
	}

	if (MO.getTargetFlags()) {
		O << ")";
	}
}

bool REDEFINEAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo, unsigned AsmVariant, const char *ExtraCode, raw_ostream &OS) {
	if (ExtraCode && *ExtraCode == 'n') {
		if (!MI->getOperand(OpNo).isImm())
			return true;
		OS << -int64_t(MI->getOperand(OpNo).getImm());
	} else {
		printOperand(MI, OpNo, OS);
	}
	return false;
}

bool REDEFINEAsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNo, unsigned AsmVariant, const char *ExtraCode, raw_ostream &OS) {
	REDEFINEInstPrinter::printAddress(MI->getOperand(OpNo).getReg(), MI->getOperand(OpNo + 1).getImm(), OS);
	return false;
}

void REDEFINEAsmPrinter::printMemOperand(const MachineInstr *MI, int opNum, raw_ostream &OS) {
	errs()<<"printing mem operand!!\n";
	OS << '%' << REDEFINEInstPrinter::getRegisterName(MI->getOperand(opNum).getReg());
	OS << ",";
	OS << MI->getOperand(opNum + 1).getImm();
}

void REDEFINEAsmPrinter::EmitEndOfAsmFile(Module &M) {
	if (Subtarget->isTargetELF()) {
		const TargetLoweringObjectFileELF &TLOFELF = static_cast<const TargetLoweringObjectFileELF &>(getObjFileLowering());

		MachineModuleInfoELF &MMIELF = MMI->getObjFileInfo<MachineModuleInfoELF>();

		// Output stubs for external and common global variables.
		MachineModuleInfoELF::SymbolListTy Stubs = MMIELF.GetGVStubList();
		if (!Stubs.empty()) {
			OutStreamer.SwitchSection(TLOFELF.getDataRelSection());
			const DataLayout *TD = TM.getDataLayout();

			for (unsigned i = 0, e = Stubs.size(); i != e; ++i) {
				OutStreamer.EmitLabel(Stubs[i].first);
				OutStreamer.EmitSymbolValue(Stubs[i].second.getPointer(), TD->getPointerSize(0), 0);
			}
			Stubs.clear();
		}
	}
}

// Force static initialization.
extern "C" void LLVMInitializeREDEFINEAsmPrinter() {
	RegisterAsmPrinter<REDEFINEAsmPrinter> X(TheREDEFINETarget);
}

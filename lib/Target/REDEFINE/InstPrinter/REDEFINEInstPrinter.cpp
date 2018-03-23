//===-- REDEFINEInstPrinter.cpp - Convert REDEFINE MCInst to assembly syntax ===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"

#include "REDEFINEInstPrinter.h"
#include "REDEFINEInstrInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#include "REDEFINEGenAsmWriter.inc"

void REDEFINEInstPrinter::printAddress(unsigned Base, int64_t Disp, raw_ostream &O) {
	O << Disp;
	if (Base) {
		O << '(';
		O << getRegisterName(Base) << ')';
	}
}

static void printExpr(const MCExpr *Expr, raw_ostream &OS) {
	int Offset = 0;
	const MCSymbolRefExpr *SRE;

	if (const MCBinaryExpr *BE = dyn_cast<MCBinaryExpr>(Expr)) {
		SRE = dyn_cast<MCSymbolRefExpr>(BE->getLHS());
		const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(BE->getRHS());
		assert(SRE && CE && "Binary expression must be sym+const.");
		Offset = CE->getValue();
	} else if (!(SRE = dyn_cast<MCSymbolRefExpr>(Expr)))
		assert(false && "Unexpected MCExpr type.");

	MCSymbolRefExpr::VariantKind Kind = SRE->getKind();

//	switch (Kind) {
//	default:
//		llvm_unreachable("Invalid kind!");
//	case MCSymbolRefExpr::VK_None:
//		break;
//	case MCSymbolRefExpr::VK_Mips_ABS_HI:
//		OS << "\"%hi(";
//		break;
//	case MCSymbolRefExpr::VK_Mips_ABS_LO:
//		OS << "\"%lo(";
//		break;
//	case MCSymbolRefExpr::VK_Mips_TPREL_HI:
//		OS << "%tprel_hi(";
//		break;
//	case MCSymbolRefExpr::VK_Mips_TPREL_LO:
//		OS << "%tprel_lo(";
//		break;
//	}

	OS << SRE->getSymbol();

	if (Offset) {
		if (Offset > 0)
			OS << '+';
		OS << Offset;
	}

	if (Kind != MCSymbolRefExpr::VK_None)
		OS << ")\"";
}

void REDEFINEInstPrinter::printOperand(const MCOperand &MC, raw_ostream &O) {
	if (MC.isReg())
		O << getRegisterName(MC.getReg());
	else if (MC.isImm())
		O << MC.getImm();
	else if (MC.isExpr())
		printExpr(MC.getExpr(), O);
	else
		llvm_unreachable("Invalid operand");
}

void REDEFINEInstPrinter::printInst(const MCInst *MI, raw_ostream &O, StringRef Annot) {
	errs()<<"printing instruction:";
	MI->dump();
	printInstruction(MI, O);
	printAnnotation(O, Annot);
}

void REDEFINEInstPrinter::printRegName(raw_ostream &O, unsigned RegNo) const {
	O << getRegisterName(RegNo);
}

void REDEFINEInstPrinter::printMemOperand(const MCInst *MI, int opNum, raw_ostream &OS) {
	//TODO terrible terrible TERRIBLE hack; Bad bad Kavitha
	static bool firstOperand=true;
	//First operand
	if(firstOperand){
		firstOperand = false;
		OS << getRegisterName(MI->getOperand(opNum).getReg());
	}else {
		//Assuming there are only 2 operands
		printOperand(MI, opNum + 1, OS);
		//I know, lame right?
		firstOperand = true;
	}
//	//TODO Head revision:
//	printOperand(MI, opNum + 1, OS);
//	OS << "(";
//	OS << getRegisterName(MI->getOperand(opNum).getReg());
//	OS << ")";
}

void REDEFINEInstPrinter::printBranchTarget(const MCInst *MI, int opNum, raw_ostream &OS) {
	if (MI->getOperand(opNum).isImm()) {
		OS << ".+"; //constant branch
	}
	printOperand(MI, opNum, OS);
}

void REDEFINEInstPrinter::printMemRegOperand(const MCInst *MI, int opNum, raw_ostream &OS) {
	OS << "0"; //No offset for this ever
	OS << "(";
	OS << getRegisterName(MI->getOperand(opNum).getReg());
	OS << ")";
}

void REDEFINEInstPrinter::printS12ImmOperand(const MCInst *MI, int OpNum, raw_ostream &O) {
	if (MI->getOperand(OpNum).isImm()) {
		int64_t Value = MI->getOperand(OpNum).getImm();
		assert(isInt<12>(Value) && "Invalid s12imm argument");
		O << Value;
	} else
		printOperand(MI, OpNum, O);
}

void REDEFINEInstPrinter::printU12ImmOperand(const MCInst *MI, int OpNum, raw_ostream &O) {
	if (MI->getOperand(OpNum).isImm()) {
		int64_t Value = MI->getOperand(OpNum).getImm();
		assert(isUInt<12>(Value) && "Invalid u12imm argument");
		O << Value;
	} else
		printOperand(MI, OpNum, O);
}

void REDEFINEInstPrinter::printS20ImmOperand(const MCInst *MI, int OpNum, raw_ostream &O) {
	if (MI->getOperand(OpNum).isImm()) {
		int64_t Value = MI->getOperand(OpNum).getImm();
		assert(isInt<20>(Value) && "Invalid s20imm argument");
		O << Value;
	} else
		printOperand(MI, OpNum, O);
}

void REDEFINEInstPrinter::printU20ImmOperand(const MCInst *MI, int OpNum, raw_ostream &O) {
	if (MI->getOperand(OpNum).isImm()) {
		int64_t Value = MI->getOperand(OpNum).getImm();
		assert(isUInt<20>(Value) && "Invalid u20imm argument");
		O << Value;
	} else
		printOperand(MI, OpNum, O);
}

void REDEFINEInstPrinter::printS32ImmOperand(const MCInst *MI, int OpNum, raw_ostream &O) {
	if (MI->getOperand(OpNum).isImm()) {
		int64_t Value = MI->getOperand(OpNum).getImm();
		assert(isInt<32>(Value) && "Invalid s32imm argument");
		O << Value;
	} else
		printOperand(MI, OpNum, O);
}

void REDEFINEInstPrinter::printU32ImmOperand(const MCInst *MI, int OpNum, raw_ostream &O) {
	if (MI->getOperand(OpNum).isImm()) {
		int64_t Value = MI->getOperand(OpNum).getImm();
		assert(isUInt<32>(Value) && "Invalid u32imm argument");
		O << Value;
	} else
		printOperand(MI, OpNum, O);
}

void REDEFINEInstPrinter::printAccessRegOperand(const MCInst *MI, int OpNum, raw_ostream &O) {
	uint64_t Value = MI->getOperand(OpNum).getImm();
	assert(Value < 16 && "Invalid access register number");
	O << "%a" << (unsigned int) Value;
}

void REDEFINEInstPrinter::printOperand(const MCInst *MI, int OpNum, raw_ostream &O) {
	printOperand(MI->getOperand(OpNum), O);
}

void REDEFINEInstPrinter::printCond4Operand(const MCInst *MI, int OpNum, raw_ostream &O) {
	static const char * const CondNames[] = { "o", "h", "nle", "l", "nhe", "lh", "ne", "e", "nlh", "he", "nl", "le", "nh", "no" };
	uint64_t Imm = MI->getOperand(OpNum).getImm();
	assert(Imm > 0 && Imm < 15 && "Invalid condition");
	O << CondNames[Imm - 1];
}

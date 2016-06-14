//===-- REDEFINEMCInstLower.cpp - Lower MachineInstr to MCInst -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "REDEFINEMCInstLower.h"
#include "REDEFINEAsmPrinter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Target/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "REDEFINEUtils.h"
#include "llvm/ADT/StringExtras.h"
using namespace llvm;

// Where relaxable pairs of reloc-generating instructions exist,
// we tend to use the longest form by default, since that produces
// correct assembly in cases where no relaxation is performed.
// If Opcode is one such instruction, return the opcode for the
// shortest possible form instead, otherwise return Opcode itself.
static unsigned getShortenedInstr(unsigned Opcode) {
	return Opcode;
}

// Return the VK_* enumeration for MachineOperand target flags Flags.
static MCSymbolRefExpr::VariantKind getVariantKind(unsigned Flags) {
	switch (Flags & REDEFINEII::MO_SYMBOL_MODIFIER) {
	case 0:
		return MCSymbolRefExpr::VK_None;
	case REDEFINEII::MO_GOT:
		return MCSymbolRefExpr::VK_GOT;
	}
	llvm_unreachable("Unrecognised MO_ACCESS_MODEL");
}

REDEFINEMCInstLower::REDEFINEMCInstLower(Mangler *mang, MCContext &ctx, REDEFINEAsmPrinter &asmprinter) :
		Mang(mang), Ctx(ctx), AsmPrinter(asmprinter) {
}

MCOperand REDEFINEMCInstLower::lowerSymbolOperand(const MachineOperand &MO, const MCSymbol *Symbol, int64_t Offset) const {
	MCSymbolRefExpr::VariantKind Kind = getVariantKind(MO.getTargetFlags());
	switch (MO.getTargetFlags()) {
	case REDEFINEII::MO_ABS_HI:
		Kind = MCSymbolRefExpr::VK_Mips_ABS_HI;
		break;
	case REDEFINEII::MO_ABS_LO:
		Kind = MCSymbolRefExpr::VK_Mips_ABS_LO;
		break;
//	case REDEFINEII::MO_TPREL_HI:
//		Kind = MCSymbolRefExpr::VK_Mips_TPREL_HI;
//		break;
//	case REDEFINEII::MO_TPREL_LO:
//		Kind = MCSymbolRefExpr::VK_Mips_TPREL_LO;
//		break;
	}
	const MCExpr *Expr = MCSymbolRefExpr::Create(Symbol, Kind, Ctx);
	if (Offset) {
		const MCExpr *OffsetExpr = MCConstantExpr::Create(Offset, Ctx);
		Expr = MCBinaryExpr::CreateAdd(Expr, OffsetExpr, Ctx);
	}
	return MCOperand::CreateExpr(Expr);
}

MCOperand REDEFINEMCInstLower::lowerOperand(const MachineOperand &MO) const {
	std::map<StringRef, long int> globalVarStartAddressMap;
	long int maxGlobalSize = 0;
	const Module* parentModule = MO.getParent()->getParent()->getParent()->getFunction()->getParent();
	for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
		const GlobalVariable *globalVar = &*globalArgItr;
		globalVarStartAddressMap.insert(std::make_pair(globalVar->getName(), maxGlobalSize));
		maxGlobalSize += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
	}
	switch (MO.getType()) {
	case MachineOperand::MO_Register:
		// Ignore all implicit register operands.
		if (MO.isImplicit())
			return MCOperand();
		return MCOperand::CreateReg(MO.getReg());

	case MachineOperand::MO_Immediate:
		return MCOperand::CreateImm(MO.getImm());

	case MachineOperand::MO_MachineBasicBlock:
		return lowerSymbolOperand(MO, MO.getMBB()->getSymbol(),
		/* MO has no offset field */0);

	case MachineOperand::MO_GlobalAddress: {
//		return MCOperand::CreateImm(globalVarStartAddressMap.find(MO.getGlobal()->getName())->second + MO.getOffset());
		string globalName = "ga#";
		globalName.append(itostr(globalVarStartAddressMap.find(MO.getGlobal()->getName())->second + MO.getOffset()));
		return lowerSymbolOperand(MO, Ctx.GetOrCreateSymbol(StringRef(globalName)), 0);
	}

	case MachineOperand::MO_ExternalSymbol: {
		StringRef Name = MO.getSymbolName();
		return lowerSymbolOperand(MO, AsmPrinter.GetExternalSymbolSymbol(Name), MO.getOffset());
	}
		//TODO: Somehow, getObjectOffset doesn't work, need to check why;
	case MachineOperand::MO_FrameIndex: {
		const MachineFrameInfo* frameInfo = MO.getParent()->getParent()->getParent()->getFrameInfo();
		unsigned currentObjectOffset = 0;
		if (MO.getIndex() < 0) {
			//The object is a reference being passed via memory
			if (frameInfo->getObjectIndexEnd() > 0) {
				for (int i = 0; i <= frameInfo->getObjectIndexEnd(); i++) {
					currentObjectOffset += frameInfo->getObjectSize(i);
				}
			}
			for (int i = -1; i > MO.getIndex(); i--) {
				currentObjectOffset += frameInfo->getObjectSize(i);
			}
		} else {
			for (int i = 0; i < MO.getIndex(); i++) {
				currentObjectOffset += frameInfo->getObjectSize(i);
			}
		}
		MCOperand retVal = MCOperand::CreateImm(currentObjectOffset + maxGlobalSize);
		return retVal;
	}

	case MachineOperand::MO_JumpTableIndex:
		return lowerSymbolOperand(MO, AsmPrinter.GetJTISymbol(MO.getIndex()),
		/* MO has no offset field */0);

	case MachineOperand::MO_ConstantPoolIndex:
		return lowerSymbolOperand(MO, AsmPrinter.GetCPISymbol(MO.getIndex()), MO.getOffset());

	case MachineOperand::MO_BlockAddress: {
		const BlockAddress *BA = MO.getBlockAddress();
		return lowerSymbolOperand(MO, AsmPrinter.GetBlockAddressSymbol(BA), MO.getOffset());
	}
		//Making use of MC Symbol to enable patching of HyperOp ids in fbind and createinst instructions later
	case MachineOperand::MO_MCSymbol: {
		return lowerSymbolOperand(MO, MO.getMCSymbol(), 0);
	}
	default:
		llvm_unreachable("unknown operand type");
	}
}

void REDEFINEMCInstLower::lower(const MachineInstr *MI, MCInst &OutMI) const {
	unsigned Opcode = MI->getOpcode();
	// When emitting binary code, start with the shortest form of an instruction
	// and then relax it where necessary.
	if (!AsmPrinter.OutStreamer.hasRawTextSupport())
		Opcode = getShortenedInstr(Opcode);
	OutMI.setOpcode(Opcode);
	for (unsigned I = 0, E = MI->getNumOperands(); I != E; ++I) {
		const MachineOperand &MO = MI->getOperand(I);
		MCOperand MCOp = lowerOperand(MO);
		if (MCOp.isValid())
			OutMI.addOperand(MCOp);
	}
}


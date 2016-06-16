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
#include "llvm/ADT/StringExtras.h"
#include "REDEFINEUtils.h"

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
		string codeSegmentStart = "\n.code\t;pHyperOp";
		codeSegmentStart.append(itostr(pHyperOpIndex)).append("\n");
		OutStreamer.EmitRawText(StringRef(codeSegmentStart));
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

	EmitFunctionBodyEnd();
}

void REDEFINEAsmPrinter::EmitFunctionBodyEnd() {
	HyperOpInteractionGraph * HIG = ((REDEFINETargetMachine&) TM).HIG;
	HyperOp* hyperOp = HIG->getHyperOp(const_cast<Function*>(MF->getFunction()));
	//Add instance metadata
	//TODO additional changes for instances of the same HyperOp
	string isStaticHyperOp(STATIC_HYPEROP_ANNOTATION);
	isStaticHyperOp.append("\t").append("1").append("\n");
	OutStreamer.EmitRawText(StringRef(isStaticHyperOp));

	string isValid(VALID_ANNOTATION);
	isValid.append("\t").append("1").append("\n");
	OutStreamer.EmitRawText(StringRef(isValid));

	string isActive(ACTIVE_ANNOTATION);
	if (hyperOp->isStartHyperOp()) {
		isActive.append("\t").append("0").append("\n");
	} else {
		isActive.append("\t").append("1").append("\n");
	}
	OutStreamer.EmitRawText(StringRef(isActive));

	//Every frame is a raw frame, no intrinsic frames available
	string isIntrinsic(INTRINSIC_ANNOTATION);
	isIntrinsic.append("\t").append("0").append("\n");
	OutStreamer.EmitRawText(StringRef(isIntrinsic));

	string depthHEG(DEPTH_HEG_ANNOTATION);
	depthHEG.append("\t").append(itostr(hyperOp->computeDepthInGraph())).append("\n");
	OutStreamer.EmitRawText(StringRef(depthHEG));

	string launchCount(LAUNCH_CNT_ANNOTATION);
	//TODO Should predicates be included in this?
	unsigned argCount = MF->getFunction()->arg_size();
	launchCount.append("\t").append(itostr(argCount)).append("\n");
	OutStreamer.EmitRawText(StringRef(launchCount));

	//TODO
	string operandValidity(OPERAND_VALIDITY_ANNOTATION);
	operandValidity.append("\t").append(bitset<36>(argCount).to_string()).append("\n");
	OutStreamer.EmitRawText(StringRef(operandValidity));

	//TODO
	string opWaitCount(OP_WAIT_CNT_ANNOTATION);
	opWaitCount.append("\t").append(itostr(argCount)).append("\n");
	OutStreamer.EmitRawText(StringRef(opWaitCount));

	//TODO
	string isNextHyperOpInstValid(ISNEXT_HOP_INST_VALID_ANNOTATION);
	isNextHyperOpInstValid.append("\t").append("0").append("\n");
	OutStreamer.EmitRawText(StringRef(isNextHyperOpInstValid));

	string nextHyperOpInst(NEXT_HYPEROP_INST_ANNOTATION);
	nextHyperOpInst.append("\t").append("0").append("\n");
	OutStreamer.EmitRawText(StringRef(nextHyperOpInst));

	string dataLabel = ".data\t";
	const Module* parentModule = MF->getFunction()->getParent();
	long int maxGlobalSize = 0;
	string inputs = "";
	unsigned numInputsAndOutputs = 0;
	for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
		const GlobalVariable *globalVar = &*globalArgItr;
		if (globalVar->getName().startswith("redefine_in_")) {
			//Every global is a pointer type
			inputs.append("i\t");
			inputs.append("\"ga#").append(itostr(maxGlobalSize)).append("\"").append("\n");
			numInputsAndOutputs++;

		} else if (globalVar->getName().startswith("redefine_out_")) {
			inputs.append("o\t");
			inputs.append("\"ga#").append(itostr(maxGlobalSize)).append("\"").append("\n");
			numInputsAndOutputs++;
		}	//Mark the global as both input and output
		else if (globalVar->getName().startswith("redefine_inout_")) {
			inputs.append("i\t");
			inputs.append("\"ga#").append(itostr(maxGlobalSize)).append("\"").append("\n");
			inputs.append("o\t");
			inputs.append("\"ga#").append(itostr(maxGlobalSize)).append("\"").append("\n");
			numInputsAndOutputs++;
		}
		maxGlobalSize += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
	}

	dataLabel.append(itostr(numInputsAndOutputs)).append("\n");
	OutStreamer.EmitRawText(StringRef(dataLabel));
	if (!inputs.empty()) {
		OutStreamer.EmitRawText(StringRef(inputs));
	}

}

void REDEFINEAsmPrinter::EmitFunctionEntryLabel() {
	static bool firstFunctionBeingProcessed = true;
	static list<unsigned> crWithNumHopsPrinted;
	int ceCount = ((REDEFINETargetMachine&) TM).getSubtargetImpl()->getCeCount();
	int dgmSize = ((REDEFINETargetMachine&) TM).getSubtargetImpl()->getDgm();
	HyperOpInteractionGraph * HIG = ((REDEFINETargetMachine&) TM).HIG;
	HyperOp* hyperOp = HIG->getHyperOp(const_cast<Function*>(MF->getFunction()));
//TODO couldn't find any method that gets invoked that could insert topology details
	if (firstFunctionBeingProcessed) {
		int maxXInTopology = 0, maxYInTopology = 0;
		int fabricRowCount = (((REDEFINETargetMachine&) TM).getSubtargetImpl())->getM();
		int fabricColumnCount = (((REDEFINETargetMachine&) TM).getSubtargetImpl())->getN();
		for (list<HyperOp*>::iterator hyperOpItr = HIG->Vertices.begin(); hyperOpItr != HIG->Vertices.end(); hyperOpItr++) {
			HyperOp* hyperOp = *hyperOpItr;
			int mappedToX = hyperOp->getTargetResource() / fabricRowCount;
			int mappedToY = hyperOp->getTargetResource() % fabricColumnCount;
			if (mappedToX > maxXInTopology) {
				mappedToX = maxXInTopology;
			}
			if (mappedToY > maxYInTopology) {
				mappedToY = maxYInTopology;
			}
		}

		long int maxGlobalSize = 0;
		for (Module::const_global_iterator globalArgItr = MF->getFunction()->getParent()->global_begin(); globalArgItr != MF->getFunction()->getParent()->global_end(); globalArgItr++) {
			maxGlobalSize += REDEFINEUtils::getAlignedSizeOfType(globalArgItr->getType());
		}

		while (maxGlobalSize > ((maxXInTopology + 1) * (maxYInTopology + 1) * dgmSize)) {
			if (maxXInTopology < maxYInTopology) {
				maxXInTopology++;
			} else {
				maxYInTopology++;
			}
		}

		string topology(".topology");
		topology.append("\t").append(itostr(maxXInTopology)).append("\t").append(itostr(maxYInTopology)).append("\n");
		OutStreamer.EmitRawText(StringRef(topology));
		firstFunctionBeingProcessed = false;

	}

	if (find(crWithNumHopsPrinted.begin(), crWithNumHopsPrinted.end(), hyperOp->getTargetResource()) == crWithNumHopsPrinted.end()) {
		unsigned targetResource = hyperOp->getTargetResource();
		unsigned numResourcesInTarget = 0;
		//First time dealing with the target resource
		for (list<HyperOp*>::iterator hyperOpItr = HIG->Vertices.begin(); hyperOpItr != HIG->Vertices.end(); hyperOpItr++) {
			if ((*hyperOpItr)->getTargetResource() == targetResource) {
				numResourcesInTarget++;
			}
		}
		string numHopsInCR(".numHyop_in_this_CR\t");
		numHopsInCR.append(itostr(numResourcesInTarget)).append("\n");
		OutStreamer.EmitRawText(StringRef(numHopsInCR));
		crWithNumHopsPrinted.push_back(targetResource);
	}

	string hyperOpLabel = "HyperOp#";
	hyperOpLabel.append(itostr(hyperOp->getHyperOpId())).append(":\n");
	OutStreamer.EmitRawText(StringRef(hyperOpLabel));
	string staticMetadata = ".SMD\t";
	staticMetadata.append(hyperOp->isStartHyperOp() ? "1" : "0").append("\t");
	staticMetadata.append(hyperOp->isEndHyperOp() ? "1" : "0").append("\t");
	staticMetadata.append(hyperOp->isBarrierHyperOp() ? "1" : "0").append("\t");
	staticMetadata.append(hyperOp->isPredicatedHyperOp() ? "1" : "0").append("\n");
	OutStreamer.EmitRawText(StringRef(staticMetadata));

//Adding distribution count of operands
	string distCount = ".distcnt\t";
	for (unsigned i = 0; i < ceCount; i++) {
		distCount.append(itostr(hyperOp->getNumInputsPerCE(i))).append("\t");
	}
	distCount.append("\n");
	OutStreamer.EmitRawText(StringRef(distCount));
}

void REDEFINEAsmPrinter::printOperand(const MachineInstr *MI, int OpNo, raw_ostream &O) {
	const MachineOperand &MO = MI->getOperand(OpNo);
//look at target flags to see if we should wrap this operand
	switch (MO.getTargetFlags()) {
	case REDEFINEII::MO_ABS_HI:
		O << "\"%hi(";
		break;
	case REDEFINEII::MO_ABS_LO:
		O << "\"%lo(";
		break;
//	case REDEFINEII::MO_TPREL_HI:
//		O << "%tprel_hi(";
//		break;
//	case REDEFINEII::MO_TPREL_LO:
//		O << "%tprel_lo(";
//		break;
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
		O << ")\"";
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
	OS << '%' << REDEFINEInstPrinter::getRegisterName(MI->getOperand(opNum).getReg());
	OS << ",";
	OS << MI->getOperand(opNum + 1).getImm();
}

//
//void REDEFINEAsmPrinter::EmitEndOfAsmFile(Module &M) {
//	if (Subtarget->isTargetELF()) {
//		const TargetLoweringObjectFileELF &TLOFELF = static_cast<const TargetLoweringObjectFileELF &>(getObjFileLowering());
//
//		MachineModuleInfoELF &MMIELF = MMI->getObjFileInfo<MachineModuleInfoELF>();
//
//		// Output stubs for external and common global variables.
//		MachineModuleInfoELF::SymbolListTy Stubs = MMIELF.GetGVStubList();
//		if (!Stubs.empty()) {
//			OutStreamer.SwitchSection(TLOFELF.getDataRelSection());
//			const DataLayout *TD = TM.getDataLayout();
//
//			for (unsigned i = 0, e = Stubs.size(); i != e; ++i) {
//				OutStreamer.EmitLabel(Stubs[i].first);
//				OutStreamer.EmitSymbolValue(Stubs[i].second.getPointer(), TD->getPointerSize(0), 0);
//			}
//			Stubs.clear();
//		}
//	}
//}

bool REDEFINEAsmPrinter::doInitialization(Module &M) {
	OutStreamer.InitStreamer();
	hyperOpLabelsHoistedAsConstants.clear();

	MMI = getAnalysisIfAvailable<MachineModuleInfo>();
	MMI->AnalyzeModule(M);

// Initialize TargetLoweringObjectFile.
	const_cast<TargetLoweringObjectFile&>(getObjFileLowering()).Initialize(OutContext, TM);

	Mang = new Mangler(OutContext, *TM.getDataLayout());

// Allow the target to emit any magic that it wants at the start of the file.
	EmitStartOfAsmFile(M);

// Emit module-level inline asm if it exists.
	if (!M.getModuleInlineAsm().empty()) {
		OutStreamer.AddComment("Start of file scope inline assembly");
		OutStreamer.AddBlankLine();
		EmitInlineAsm(M.getModuleInlineAsm() + "\n");
		OutStreamer.AddComment("End of file scope inline assembly");
		OutStreamer.AddBlankLine();
	}

	return false;
}

bool REDEFINEAsmPrinter::doFinalization(Module &M) {
	EmitEndOfAsmFile(M);

	delete Mang;
	Mang = 0;
	MMI = 0;

	OutStreamer.Finish();
	OutStreamer.reset();

	return false;
}

void REDEFINEAsmPrinter::EmitLinkage(unsigned Linkage, MCSymbol *GVSym) const {
//do nothing, no linkage yet
}

// Force static initialization.
extern "C" void LLVMInitializeREDEFINEAsmPrinter() {
	RegisterAsmPrinter<REDEFINEAsmPrinter> X(TheREDEFINETarget);
}


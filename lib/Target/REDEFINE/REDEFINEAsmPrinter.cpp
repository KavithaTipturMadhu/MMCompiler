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

		string codeSegmentStart = ".PHYOP#";
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
		OutStreamer.EmitRawText(StringRef(".PHYOP_END\n"));
	}

	EmitFunctionBodyEnd();
}

void REDEFINEAsmPrinter::EmitFunctionBodyEnd() {
	int ceCount = ((REDEFINETargetMachine&) TM).getSubtargetImpl()->getCeCount();
	HyperOpInteractionGraph * HIG = ((REDEFINETargetMachine&) TM).HIG;
	HyperOp* hyperOp = HIG->getHyperOp(const_cast<Function*>(MF->getFunction()));
	//Add instance metadata
	//TODO additional changes for instances of the same HyperOp
	string isStaticHyperOp(STATIC_HYPEROP_ANNOTATION);
	isStaticHyperOp.append("\t").append(hyperOp->isStaticHyperOp() ? "Y" : "N").append("\n");
	OutStreamer.EmitRawText(StringRef(isStaticHyperOp));

	if (hyperOp->isStaticHyperOp()) {
		OutStreamer.EmitRawText(StringRef(".IMD_BEGIN\n"));

		string instanceId(HYPEROP_INSTANCE_PREFIX);
		instanceId.append(itostr(hyperOp->getContextFrame() << 6)).append("\t");
		OutStreamer.EmitRawText(StringRef(instanceId));

		AttributeSet attributes = MF->getFunction()->getAttributes();
		unsigned i = 1;
		unsigned argCount = 0;
		for (Function::const_arg_iterator argItr = MF->getFunction()->arg_begin(); argItr != MF->getFunction()->arg_end(); argItr++, i++) {
			if (attributes.hasAttribute(1, Attribute::InReg) && !argItr->getType()->isPointerTy()) {
				argCount++;
			}
		}

		// Added By Arka Instance Metadata Annotations
		string instAnn(".ANN\t");
		instAnn.append("\t").append(hyperOp->isStartHyperOp() ? "" : "A").append("\t");
		instAnn.append("\t").append(hyperOp->isBarrierHyperOp() ? "B" : "").append("\t");
		instAnn.append("\t").append(hyperOp->isPredicatedHyperOp() ? "P" : "").append("\t");
		OutStreamer.EmitRawText(StringRef(instAnn));

		string depthHEG(DEPTH_HEG_ANNOTATION);
		depthHEG.append("\t").append(itostr(hyperOp->computeDepthInGraph())).append("\n");
		OutStreamer.EmitRawText(StringRef(depthHEG));

		string numphy = ".numphy\t";
		numphy.append(itostr(ceCount)).append("\n");
		OutStreamer.EmitRawText(StringRef(numphy));

		string launchCount(LAUNCH_CNT_ANNOTATION);
		launchCount.append("\t").append(itostr(argCount)).append("\n");
		OutStreamer.EmitRawText(StringRef(launchCount));

		string operandValidity(OPERAND_VALIDITY_ANNOTATION);
		operandValidity.append("\t").append(bitset<16>(argCount).to_string()).append("\n");
		OutStreamer.EmitRawText(StringRef(operandValidity));

		string opWaitCount(OP_WAIT_CNT_ANNOTATION);
		if(hyperOp->isBarrierHyperOp()) {
			opWaitCount.append("\t").append(itostr(argCount+1)).append("\n");
		}
		opWaitCount.append("\t").append(itostr(argCount)).append("\n");
		OutStreamer.EmitRawText(StringRef(opWaitCount));

		OutStreamer.EmitRawText(StringRef(".IMD_END\n"));
		OutStreamer.EmitRawText(StringRef(".HYOP_END\n\n"));
		//string isNextHyperOpInstValid(ISNEXT_HOP_INST_VALID_ANNOTATION);
		//string nextHyperOpInst(NEXT_HYPEROP_INST_ANNOTATION);
		//isNextHyperOpInstValid.append("\t").append("0").append("\n");
		//nextHyperOpInst.append("\t").append("0").append("\n");

		//OutStreamer.EmitRawText(StringRef(isNextHyperOpInstValid));
		//OutStreamer.EmitRawText(StringRef(nextHyperOpInst));
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
				maxXInTopology = mappedToX;
			}
			if (mappedToY > maxYInTopology) {
				maxYInTopology = mappedToY;
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

		// Added By Arka
		string startAddr(".startaddr"); // Start Address of the Code Data Segment
		startAddr.append("\t").append(itostr(0)).append("\n");
		OutStreamer.EmitRawText(StringRef(startAddr));
	}
	
	// Added By Arka, HyperOp Static Metadeta
	string hyperOpLabel = ".HyOp#";
	hyperOpLabel.append(itostr(hyperOp->getHyperOpId())).append(":\n");
	OutStreamer.EmitRawText(StringRef(hyperOpLabel));

	string staticMetadata = ".SMD_BEGIN\t\n";
	staticMetadata.append(".ann\t").append(hyperOp->isStartHyperOp() ? "S" : "").append("\t");
	staticMetadata.append(hyperOp->isEndHyperOp() ? "E" : "").append("\t");
	staticMetadata.append(hyperOp->isBarrierHyperOp() ? "B" : "").append("\t");
	staticMetadata.append(hyperOp->isPredicatedHyperOp() ? "P" : "").append("\n");
	OutStreamer.EmitRawText(StringRef(staticMetadata));

	string numphy = ".numphy\t";
	numphy.append(itostr(ceCount)).append("\n");
	OutStreamer.EmitRawText(StringRef(numphy));

	//Adding distribution count of operands
	string distCount = ".opdist\t";
	for (unsigned i = 0; i < ceCount; i++) {
		distCount.append(itostr(hyperOp->getNumInputsPerCE(i))).append("\t");
	}
	distCount.append("\n");
	OutStreamer.EmitRawText(StringRef(distCount));
	OutStreamer.EmitRawText(StringRef(".SMD_END"));

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

void REDEFINEAsmPrinter::EmitEndOfAsmFile(Module &M) {
	string dataLabel = ".IO_BEGIN\n.data_start\t";
	long int maxGlobalSize = 0;
	string inputs = "";
	unsigned numInputsAndOutputs = 0;
	for (Module::const_global_iterator globalArgItr = M.global_begin(); globalArgItr != M.global_end(); globalArgItr++) {
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
	string ioEndLabel = ".IO_END";
	OutStreamer.EmitRawText(StringRef(ioEndLabel));
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

void REDEFINEAsmPrinter::EmitFunctionHeader() {
	// Print out constants referenced by the function
	EmitConstantPool();

	// Print the 'header' of function.
	const Function *F = MF->getFunction();

	OutStreamer.SwitchSection(getObjFileLowering().SectionForGlobal(F, Mang, TM));

	// Emit the CurrentFnSym.  This is a virtual function to allow targets to
	// do their wild and crazy things as required.
	EmitFunctionEntryLabel();

	// If the function had address-taken blocks that got deleted, then we have
	// references to the dangling symbols.  Emit them at the start of the function
	// so that we don't get references to undefined symbols.
	std::vector<MCSymbol*> DeadBlockSyms;
	MMI->takeDeletedSymbolsForFunction(F, DeadBlockSyms);
	for (unsigned i = 0, e = DeadBlockSyms.size(); i != e; ++i) {
		OutStreamer.AddComment("Address taken block that was later removed");
		OutStreamer.EmitLabel(DeadBlockSyms[i]);
	}

}

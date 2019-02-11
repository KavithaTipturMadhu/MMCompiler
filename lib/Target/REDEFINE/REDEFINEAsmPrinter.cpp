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
#include "llvm/IR/REDEFINEUtils.h"
#include <sstream>
using namespace std;
using namespace llvm;

void REDEFINEAsmPrinter::EmitInstruction(const MachineInstr *MI) {
	REDEFINEMCInstLower Lower(Mang, MF->getContext(), *this);
	MCInst LoweredMI;
	Lower.lower(MI, LoweredMI);
	OutStreamer.EmitInstruction(LoweredMI);
}

// Convert a REDEFINE-specific constant pool modifier into the associated
// MCSymbolRefExpr variant kind.
static MCSymbolRefExpr::VariantKind getModifierVariantKind(
		REDEFINECP::REDEFINECPModifier Modifier) {
	switch (Modifier) {
	case REDEFINECP::NTPOFF:
		return MCSymbolRefExpr::VK_NTPOFF;
	}
	llvm_unreachable("Invalid SystemCPModifier!");
}

void REDEFINEAsmPrinter::EmitMachineConstantPoolValue(
		MachineConstantPoolValue *MCPV) {
	REDEFINEConstantPoolValue *ZCPV =
			static_cast<REDEFINEConstantPoolValue*>(MCPV);

	const MCExpr *Expr = MCSymbolRefExpr::Create(
			Mang->getSymbol(ZCPV->getGlobalValue()),
			getModifierVariantKind(ZCPV->getModifier()), OutContext);
	uint64_t Size = TM.getDataLayout()->getTypeAllocSize(ZCPV->getType());

	OutStreamer.EmitValue(Expr, Size);
}
void REDEFINEAsmPrinter::EmitFunctionBody() {
	int ceCount =
			((REDEFINETargetMachine&) TM).getSubtargetImpl()->getCeCount();
	// Emit target-specific gunk before the function body.
	EmitFunctionBodyStart();

	const MachineInstr *LastMI = 0;
	vector < list<const MachineInstr*> > pHyperOpInstructions(ceCount);
	vector < list<const MachineInstr*> > startOfBBInPHyperOp(ceCount);
	for (MachineFunction::const_iterator I = MF->begin(), E = MF->end(); I != E;
			++I) {
		int pHyperOpIndex = -1;
		for (MachineBasicBlock::const_instr_iterator instrItr =
				I->instr_begin(); instrItr != I->instr_end(); ++instrItr) {
			//First instruction of the pHyperOp is never in a bundle
			if (!instrItr->isInsideBundle()) {
				pHyperOpIndex++;
				startOfBBInPHyperOp[pHyperOpIndex].push_back(instrItr);
			}
			pHyperOpInstructions[pHyperOpIndex].push_back(instrItr);
		}
	}
	bool added = false;
	string codeSegmentStart = "\t.align\t16\n\t.PHYPEROP\t";
	string funcnamewithoutperiod("");
	const char* tempstring = MF->getFunction()->getName().data();
	int i = 0;
	while (tempstring[i] != '\0') {
		if (tempstring[i] == '.') {
			funcnamewithoutperiod.append(string("_"));
		} else {
			char value[2] = { tempstring[i], '\0' };
			funcnamewithoutperiod.append(string(value));
		}
		i++;
	}
	for (int pHyperOpIndex = 0; pHyperOpIndex < pHyperOpInstructions.size();
			pHyperOpIndex++) {
		if (added) {
			codeSegmentStart.append(",\t");
		}

		codeSegmentStart.append(".PC_").append(funcnamewithoutperiod).append(
				itostr(pHyperOpIndex)); //p-HyperOp-PC label
		added = true;
	}
	codeSegmentStart.append("\n");
	OutStreamer.EmitRawText(StringRef(codeSegmentStart));

//	string hyperOpId(itostr(hyperOp->getHyperOpId()));
	for (int pHyperOpIndex = 0; pHyperOpIndex < pHyperOpInstructions.size();
			pHyperOpIndex++) {
		list<const MachineInstr*> pHyperOpItr =
				pHyperOpInstructions[pHyperOpIndex];
		string codeSegmentStart(".");
		codeSegmentStart.append("PC_").append(funcnamewithoutperiod).append(
				itostr(pHyperOpIndex)).append(":\n"); //p-HyperOp-PC label
		OutStreamer.EmitRawText(StringRef(codeSegmentStart));

		for (list<const MachineInstr*>::iterator mcItr = pHyperOpItr.begin();
				mcItr != pHyperOpItr.end(); mcItr++) {
			if (!startOfBBInPHyperOp[pHyperOpIndex].empty()
					&& startOfBBInPHyperOp[pHyperOpIndex].front() == *mcItr) {
				MCSymbol *label = (*mcItr)->getParent()->getSymbol();
				label->setUndefined();
				OutStreamer.EmitLabel(label);
				startOfBBInPHyperOp[pHyperOpIndex].pop_front();
			}
			EmitInstruction(*mcItr);
		}
	}

	//The object is a reference being passed via memory
	int mfFrameSize = 0;
	if (MF->getFrameInfo()->getObjectIndexEnd() > 0) {
		for (int i = 0; i < MF->getFrameInfo()->getObjectIndexEnd(); i++) {
			mfFrameSize += REDEFINEUtils::getSizeOfType(
					MF->getFrameInfo()->getObjectAllocation(i)->getType());
		}
	}
	if (mfFrameSize > maxFrameValue) {
		maxFrameValue = mfFrameSize;
	}

	if (MF->getFunctionNumber()
			== (MF->getFunction())->getParent()->getFunctionList().size() - 2) {
		string maxFrameString;
		OutStreamer.EmitRawText(StringRef(maxFrameString));
	}
	EmitFunctionBodyEnd();
}

void REDEFINEAsmPrinter::EmitFunctionBodyEnd() {
	int ceCount =
			((REDEFINETargetMachine&) TM).getSubtargetImpl()->getCeCount();
	HyperOpInteractionGraph * HIG = ((REDEFINETargetMachine&) TM).HIG;
	HyperOp* hyperOp = HIG->getHyperOp(
			const_cast<Function*>(MF->getFunction()));
	//Add instance metadata

	if (hyperOp->isStaticHyperOp()) {
		OutStreamer.EmitRawText(StringRef("\t.STATICINSTANCE_BEGIN\n"));
		string instanceId = ("\t.INSTID ");
		instanceId.append(hyperOp->getFunction()->getName()).append("\n");
		instanceId.append("\t.INSTOF ").append("HyOp#").append(itostr(hyperOp->getHyperOpId())).append(
				"\n");
		instanceId.append("\t.INSTADDR ").append(
				itostr(hyperOp->getTargetResource())).append(",").append(itostr(hyperOp->getContextFrame()*64)).append("\n");
		OutStreamer.EmitRawText(StringRef(instanceId));

		string instAnn("\t.ANNO I");
		instAnn.append(hyperOp->isStartHyperOp() ? ",A" : "").append("\n");
		OutStreamer.EmitRawText(StringRef(instAnn));

		if(hyperOp->isStartHyperOp()){
			string startOperand("\t.OPERAND 15, 1\n");
			OutStreamer.EmitRawText(StringRef(startOperand));
		}
		OutStreamer.EmitRawText(StringRef("\t.STATICINSTANCE_END\n"));
	}
}

void REDEFINEAsmPrinter::EmitFunctionEntryLabel() {
	int ceCount =
			((REDEFINETargetMachine&) TM).getSubtargetImpl()->getCeCount();
	HyperOpInteractionGraph * HIG = ((REDEFINETargetMachine&) TM).HIG;
	HyperOp* hyperOp = HIG->getHyperOp(
			const_cast<Function*>(MF->getFunction()));

	OutStreamer.EmitRawText(";" + StringRef(MF->getFunction()->getName()));

	string staticMetadata = "\n\t.align 16\n\t.SMD_BEGIN\t\n";
	string hyperOpLabel = "\t\t.HYPEROPID\t.";
	hyperOpLabel.append("HyOp#").append(itostr(hyperOp->getHyperOpId())).append(
			"\n");
	staticMetadata.append(StringRef(hyperOpLabel));

	if (hyperOp->isEndHyperOp() || hyperOp->isStartHyperOp()
			|| hyperOp->isBarrierHyperOp() || hyperOp->isPredicatedHyperOp()) {
		staticMetadata.append("\t\t.anno\t");
		bool added = false;

		if (hyperOp->isStartHyperOp()) {
			staticMetadata.append("S");
			added = true;
		}
		if (hyperOp->isEndHyperOp()) {
			if (added) {
				staticMetadata.append(",");
			}
			staticMetadata.append("E");
			added = true;
		}
		if (hyperOp->isBarrierHyperOp()) {
			if (added) {
				staticMetadata.append(",");
			}
			staticMetadata.append("B");
			added = true;
		}
		if (hyperOp->isPredicatedHyperOp()) {
			if (added) {
				staticMetadata.append(",");
			}
			staticMetadata.append("P");
		}
		staticMetadata.append("\n");
	}

	staticMetadata.append("\t\t.numphy\t");
	staticMetadata.append(itostr(ceCount)).append("\n");

	AttributeSet attributes = MF->getFunction()->getAttributes();
	unsigned i = 1;
	unsigned argCount = 0;

	for (Function::const_arg_iterator argItr = MF->getFunction()->arg_begin();
			argItr != MF->getFunction()->arg_end(); argItr++, i++) {
		if (attributes.hasAttribute(i, Attribute::InReg)
				&& !argItr->getType()->isPointerTy()) {
			argCount++;
		}
	}
	//Add context frame addresses and ordering edges also
	for (map<HyperOpEdge*, HyperOp*>::iterator parentMapItr =
			hyperOp->ParentMap.begin();
			parentMapItr != hyperOp->ParentMap.end(); parentMapItr++) {
		if (parentMapItr->first->getType()
				== HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR) {
			argCount++;
		}
	}
	if (hyperOp->isBarrierHyperOp()) {
		argCount++;
	}
	staticMetadata.append("\t\t.numOperand\t").append(itostr(argCount)).append(
			"\n");

	//Adding distribution count of operands
	string distCount = "\t\t.opdist\t";
	string phopPC = "\t\t.pHyPC\t";

	string funcnamewithoutperiod("");
	const char* tempstring = hyperOp->getFunction()->getName().data();
	unsigned j = 0;
	while (tempstring[j] != '\0') {
		if (tempstring[j] == '.') {
			funcnamewithoutperiod.append(string("_"));
		} else {
			char value[2] = { tempstring[j], '\0' };
			funcnamewithoutperiod.append(string(value));
		}
		j++;
	}

	vector<int> numInputsPerCE = (((REDEFINETargetMachine&)TM).pHyperOpAndNumInputsPerCE)[hyperOp->getFunction()];
	for (unsigned i = 0; i < ceCount; i++) {
		if (i > 0) {
			distCount.append(",\t");
			phopPC.append(",\T");
		}
		distCount.append(itostr(numInputsPerCE[i]));
		phopPC.append(".PC_").append(funcnamewithoutperiod).append(itostr(i));
	}
	distCount.append("\n");
	phopPC.append("\n");

	staticMetadata.append(phopPC);
	staticMetadata.append("\t.SMD_END");
	OutStreamer.EmitRawText(StringRef(staticMetadata));
}

void REDEFINEAsmPrinter::printOperand(const MachineInstr *MI, int OpNo,
		raw_ostream &O) {
	const MachineOperand &MO = MI->getOperand(OpNo);
//look at target flags to see if we should wrap this operand
//	switch (MO.getTargetFlags()) {
//	case REDEFINEII::MO_ABS_HI:
//		O << "\"%hi(";
//		break;
//	case REDEFINEII::MO_ABS_LO:
//		O << "\"%lo(";
//		break;
//	case REDEFINEII::MO_TPREL_HI:
//		O << "%tprel_hi(";
//		break;
//	case REDEFINEII::MO_TPREL_LO:
//		O << "%tprel_lo(";
//		break;
//	}
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

bool REDEFINEAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
		unsigned AsmVariant, const char *ExtraCode, raw_ostream &OS) {
	if (ExtraCode && *ExtraCode == 'n') {
		if (!MI->getOperand(OpNo).isImm())
			return true;
		OS << -int64_t(MI->getOperand(OpNo).getImm());
	} else {
		printOperand(MI, OpNo, OS);
	}
	return false;
}

bool REDEFINEAsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI,
		unsigned OpNo, unsigned AsmVariant, const char *ExtraCode,
		raw_ostream &OS) {
	REDEFINEInstPrinter::printAddress(MI->getOperand(OpNo).getReg(),
			MI->getOperand(OpNo + 1).getImm(), OS);
	return false;
}

void REDEFINEAsmPrinter::printMemOperand(const MachineInstr *MI, int opNum,
		raw_ostream &OS) {
	OS << '%'
			<< REDEFINEInstPrinter::getRegisterName(
					MI->getOperand(opNum).getReg());
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
	const_cast<TargetLoweringObjectFile&>(getObjFileLowering()).Initialize(
			OutContext, TM);

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

void addGlobalSymTab(const GlobalVariable* global, const Constant * initializer, string *assemblyString) {
		//TODO Constant cannot be vector or blockaddress
		Type *type = initializer->getType();
		if (!type->isAggregateType()) {
			assert(!isa<ConstantExpr>(initializer) && "Global expressions not supported");
			if (isa<ConstantInt>(initializer) || isa<ConstantFP>(initializer) || initializer->isZeroValue()|| isa<UndefValue>(initializer)) {
				string newglobalstring("");
				if(isa<ConstantInt>(initializer))
					newglobalstring.append("\t.word32\t").append(itostr(initializer->getUniqueInteger().getSExtValue())).append("\n");
				else if(isa<ConstantFP>(initializer)){
					ostringstream floatStr("");
					floatStr<<((ConstantFP*)initializer)->getValueAPF().convertToFloat();
					newglobalstring.append("\t.float32\t").append(floatStr.str()).append("\n");
				}
				else if(initializer->isZeroValue()|| isa<UndefValue>(initializer))
					newglobalstring.append("\t.word32\t").append(itostr(0)).append("\n");
				(*assemblyString).append(newglobalstring);
			}
		} else if (type->isArrayTy()) {
			for (unsigned i = 0; i < type->getArrayNumElements(); i++) {
				const Constant* subTypeInitializer;
				//Find the subtype's initializer
				if (const ConstantDataSequential *CDS = dyn_cast<ConstantDataSequential>(initializer)) {
					subTypeInitializer = CDS->getElementAsConstant(i);
				} else if (initializer->getNumOperands() > i) {
					subTypeInitializer = cast<Constant>(initializer->getOperand(i));
				} else if (const ConstantAggregateZero *CDS = dyn_cast<ConstantAggregateZero>(initializer)) {
					subTypeInitializer = CDS->getElementValue(i);
				} else {
					subTypeInitializer = initializer;
				}
				addGlobalSymTab(global, subTypeInitializer, assemblyString);
			}
		} else {
			for (unsigned subTypeIndex = 0; subTypeIndex < type->getNumContainedTypes(); subTypeIndex++) {
				Type* subType = type->getContainedType(subTypeIndex);
				const Constant* subTypeInitializer;
				//Find the subtype's initializer
				if (const ConstantDataSequential *CDS = dyn_cast<ConstantDataSequential>(initializer)) {
					subTypeInitializer = CDS->getElementAsConstant(subTypeIndex);
				} else if (initializer->getNumOperands() > subTypeIndex) {
					subTypeInitializer = cast<Constant>(initializer->getOperand(subTypeIndex));
				} else if (const ConstantAggregateZero *CDS = dyn_cast<ConstantAggregateZero>(initializer)) {
					subTypeInitializer = CDS->getElementValue(subTypeIndex);
				} else {
					subTypeInitializer = initializer;
				}
				addGlobalSymTab(global, subTypeInitializer, assemblyString);
			}
		}
	}

void REDEFINEAsmPrinter::EmitEndOfAsmFile(Module &M) {
	string inputs = ".DATA\n";
	unsigned numInputsAndOutputs = 0;
	for (Module::const_global_iterator globalArgItr = M.global_begin(); globalArgItr != M.global_end(); globalArgItr++, numInputsAndOutputs++) {
		const GlobalVariable *globalVar = &*globalArgItr;
		assert(globalVar->hasInitializer() && "Global without initializer does not work");
		//Every global is a pointer type
		globalVar->getInitializer()->dump();
		addGlobalSymTab(globalVar, globalVar->getInitializer(), &inputs);
	}

	OutStreamer.EmitRawText(StringRef(inputs));
	string ioEndLabel = ".END\n";
	OutStreamer.EmitRawText(StringRef(ioEndLabel));
	string maxFrameString;
	maxFrameString.append(";FS = ").append(itostr(maxFrameValue)).append("\n");
	OutStreamer.EmitRawText(StringRef(maxFrameString));
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

	OutStreamer.SwitchSection(
			getObjFileLowering().SectionForGlobal(F, Mang, TM));

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

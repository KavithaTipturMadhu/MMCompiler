//===-- REDEFINEISelDAGToDAG.cpp - A dag to dag inst selector for REDEFINE --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the REDEFINE target.
//
//===----------------------------------------------------------------------===//
#include "llvm/IR/HyperOpInteractionGraph.h"
#include "llvm/IR/HyperOpMetadataParser.h"
#include "llvm/ADT/ilist.h"
#include "llvm/ADT/ilist_node.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/Analysis.h"
#include "llvm/CodeGen/DAGCombine.h"
#include "llvm/CodeGen/FastISel.h"
#include "llvm/CodeGen/FunctionLoweringInfo.h"
#include "llvm/CodeGen/GCMetadata.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/SelectionDAGBuilder.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/CodeGen/ScheduleDAGSDNodes.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Value.h"
#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/Support/CFG.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Timer.h"
#include "llvm/Target/TargetCallingConv.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Target/TargetLibraryInfo.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/Target/TargetSubtargetInfo.h"
#include "REDEFINEISelLowering.h"
#include "REDEFINESubtarget.h"
#include "REDEFINETargetMachine.h"
#include "REDEFINESelectionDAGBuilder.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
using namespace llvm;

#include <list>
#include <utility>
using namespace std;

namespace {

// Used to build addressing modes.
struct REDEFINEAddressingMode {
	// The shape of the address.
	enum AddrForm {
		// base+offset
		FormBO
	};
	AddrForm Form;

	// The type of displacement.
	enum OffRange {
		Off12Only
	};
	OffRange OffR;

	// The parts of the address.  The address is equivalent to:
	//
	//     Base + Offset + Index + (IncludesDynAlloc ? ADJDYNALLOC : 0)
	SDValue Base;
	int64_t Offset;

	REDEFINEAddressingMode(AddrForm form, OffRange offr) :
			Form(form), OffR(offr), Base(), Offset(0) {
	}

	void dump() {
		errs() << "REDEFINEAddressingMode " << this << '\n';

		errs() << " Base ";
		if (Base.getNode() != 0)
			Base.getNode()->dump();
		else
			errs() << "null\n";

		errs() << " Offset " << Offset;
	}
};

class REDEFINEDAGToDAGISel: public SelectionDAGISel {
	const REDEFINETargetLowering &Lowering;
	const REDEFINESubtarget &Subtarget;

	// Used by REDEFINEOperands.td to create integer constants.
	inline SDValue getImm(const SDNode *Node, uint64_t Imm) {
		return CurDAG->getTargetConstant(Imm, Node->getValueType(0));
	}
	/// getI32Imm - Return a target constant with the specified value, of type
	/// i32.
	SDValue getI32Imm(unsigned Imm) {
		return CurDAG->getTargetConstant(Imm, MVT::i32);
	}

	// Try to fold more of the base or index of AM into AM, where IsBase
	// selects between the base and index.
	bool expandAddress(REDEFINEAddressingMode &AM, bool IsBase);

	// Try to describe N in AM, returning true on success.
	bool selectAddress(SDValue N, REDEFINEAddressingMode &AM);

	// Extract individual target operands from matched address AM.
	void getAddressOperands(const REDEFINEAddressingMode &AM, EVT VT, SDValue &Base, SDValue &Disp);
	void getAddressOperands(const REDEFINEAddressingMode &AM, EVT VT, SDValue &Base, SDValue &Disp, SDValue &Index);

	//REDEFINE
	bool selectMemRegAddr(SDValue Addr, SDValue &Base, SDValue &Offset) {

		EVT ValTy = Addr.getValueType();

		if (TM.getRelocationModel() != Reloc::PIC_) {
			if ((Addr.getOpcode() == ISD::TargetExternalSymbol || Addr.getOpcode() == ISD::TargetGlobalAddress))
				return false;
		}

		// Addresses of the form FI+const or FI|const
		if (CurDAG->isBaseWithConstantOffset(Addr)) {
			ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1));
			if (isInt<12>(CN->getSExtValue())) {

				// If the first operand is a FI, get the TargetFI Node
				if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr.getOperand(0))) {
					Select(FIN);
					Base = (Addr.getOperand(0));
//					Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
				} else {
					Base = Addr.getOperand(0);
				}

				Offset = CurDAG->getTargetConstant(CN->getZExtValue(), ValTy);
				return true;
			}
		}

		//Last case
		Base = Addr;
		Offset = CurDAG->getTargetConstant(0, Addr.getValueType());
		return true;
	}

	bool selectRegAddr(SDValue Addr, SDValue &Base) {
		//always just register
		Base = Addr;
		//Offset = CurDAG->getTargetConstant(0, Addr.getValueType());
		return true;
	}

	// PC-relative address matching routines used by REDEFINEOperands.td.
	bool selectPCRelAddress(SDValue Addr, SDValue &Target) {
		return false;
	}

	// If Op0 is null, then Node is a constant that can be loaded using:
	//
	//   (Opcode UpperVal LowerVal)
	//
	// If Op0 is nonnull, then Node can be implemented using:
	//
	//   (Opcode (Opcode Op0 UpperVal) LowerVal)
	SDNode *splitLargeImmediate(unsigned Opcode, SDNode *Node, SDValue Op0, uint64_t UpperVal, uint64_t LowerVal);

public:
	REDEFINEDAGToDAGISel(REDEFINETargetMachine &TM, CodeGenOpt::Level OptLevel) :
			SelectionDAGISel(TM, OptLevel), Lowering(*TM.getTargetLowering()), Subtarget(*TM.getSubtargetImpl()) {
		SDB = new REDEFINESelectionDAGBuilder(*CurDAG, *FuncInfo, OptLevel);
	}

	// Override MachineFunctionPass.
	virtual const char *getPassName() const LLVM_OVERRIDE {
		return "REDEFINE DAG->DAG Pattern Instruction Selection";
	}

	// Override SelectionDAGISel.
	virtual bool runOnMachineFunction(MachineFunction &MF);
	virtual SDNode *Select(SDNode *Node) LLVM_OVERRIDE;
	virtual bool SelectInlineAsmMemoryOperand(const SDValue &Op, char ConstraintCode, std::vector<SDValue> &OutOps) LLVM_OVERRIDE;
	void SelectBasicBlock(BasicBlock::const_iterator Begin, BasicBlock::const_iterator End, bool &HadTailCall);

	// Include the pieces autogenerated from the target description.
#include "REDEFINEGenDAGISel.inc"
};
} // end anonymous namespace

bool REDEFINEDAGToDAGISel::runOnMachineFunction(MachineFunction &mf) {
	checkCLFlags();
	Function *Fn = const_cast<Function*>(mf.getFunction());
	const TargetInstrInfo &TII = *TM.getInstrInfo();
	const TargetRegisterInfo &TRI = *TM.getRegisterInfo();
	static unsigned firstFunction = 0;
	//Add instructions to write to context frames if the function is the first one being dealt with
	if (firstFunction == 0) {
		//Parse the HIG metadata the first time, subsequent HyperOps can use the graph
		HyperOpMetadataParser parser;
		((REDEFINETargetMachine&) TM).HIG = parser.parseMetadata(const_cast<Module*>(Fn->getParent()));
		((REDEFINETargetMachine&) TM).HIG->removeUnreachableHops();
		((REDEFINETargetMachine&) TM).HIG->setDimensions((((REDEFINETargetMachine&) TM).getSubtargetImpl())->getM(), (((REDEFINETargetMachine&) TM).getSubtargetImpl())->getN());
		((REDEFINETargetMachine&) TM).HIG->setNumContextFrames((((REDEFINETargetMachine&) TM).getSubtargetImpl())->getCfCount());
		((REDEFINETargetMachine&) TM).HIG->setMaxContextFrameSize((((REDEFINETargetMachine&) TM).getSubtargetImpl())->getCfSize());
		((REDEFINETargetMachine&) TM).HIG->computeDominatorInfo();
		((REDEFINETargetMachine&) TM).HIG->makeGraphStructured();
		((REDEFINETargetMachine&) TM).HIG->computeDominatorInfo();
		((REDEFINETargetMachine&) TM).HIG->addContextFrameblockSizeEdges();
		((REDEFINETargetMachine&) TM).HIG->addContextFrameAddressForwardingEdges();
		((REDEFINETargetMachine&) TM).HIG->minimizeControlEdges();
		((REDEFINETargetMachine&) TM).HIG->clusterNodes();
		((REDEFINETargetMachine&) TM).HIG->associateStaticContextFrames();
		((REDEFINETargetMachine&) TM).HIG->mapClustersToComputeResources();
		((REDEFINETargetMachine&) TM).HIG->updateLocalRefEdgeMemOffset();
		((REDEFINETargetMachine&) TM).HIG->verify();
		firstFunction = 1;
	}
	return SelectionDAGISel::runOnMachineFunction(mf);
}

FunctionPass *llvm::createREDEFINEISelDag(REDEFINETargetMachine &TM, CodeGenOpt::Level OptLevel) {
	REDEFINEDAGToDAGISel *isel = new REDEFINEDAGToDAGISel(TM, OptLevel);
	((REDEFINETargetMachine&) TM).FuncInfo = isel->FuncInfo;
	return isel;
}

// Return true if Val should be selected as a displacement for an address
// with range DR.  Here we're interested in the range of both the instruction
// described by DR and of any pairing instruction.
static bool selectOffset(REDEFINEAddressingMode::OffRange OffR, int64_t Val) {
	switch (OffR) {
	case REDEFINEAddressingMode::Off12Only:
		return isInt<12>(Val);
	}
	llvm_unreachable("Unhandled offset range");
}

// The base or index of AM is equivalent to Op0 + Op1, where IsBase selects
// between the base and index.  Try to fold Op1 into AM's displacement.
static bool expandOffset(REDEFINEAddressingMode &AM, bool IsBase, SDValue Op0, ConstantSDNode *Op1) {
	// First try adjusting the displacement.
	int64_t TestOffset = AM.Offset + Op1->getSExtValue();
	if (selectOffset(AM.OffR, TestOffset)) {
		//changeComponent(AM, IsBase, Op0);
		AM.Base = Op0;
		AM.Offset = TestOffset;
		return true;
	}

	// We could consider forcing the displacement into a register and
	// using it as an index, but it would need to be carefully tuned.
	return false;
}

bool REDEFINEDAGToDAGISel::expandAddress(REDEFINEAddressingMode &AM, bool IsBase) {
	//SDValue N = IsBase ? AM.Base : AM.Index;
	SDValue N = AM.Base;
	unsigned Opcode = N.getOpcode();
	if (Opcode == ISD::TRUNCATE) {
		N = N.getOperand(0);
		Opcode = N.getOpcode();
	}
	if (Opcode == ISD::ADD || CurDAG->isBaseWithConstantOffset(N)) {
		SDValue Op0 = N.getOperand(0);
		SDValue Op1 = N.getOperand(1);

		unsigned Op0Code = Op0->getOpcode();
		unsigned Op1Code = Op1->getOpcode();

		if (Op0Code == ISD::Constant)
			return expandOffset(AM, IsBase, Op1, cast<ConstantSDNode>(Op0));
		if (Op1Code == ISD::Constant)
			return expandOffset(AM, IsBase, Op0, cast<ConstantSDNode>(Op1));

	}
	return false;
}

// Return true if an instruction with displacement range DR should be
// used for displacement value Val.  selectDisp(DR, Val) must already hold.
static bool isValidOffset(REDEFINEAddressingMode::OffRange OffR, int64_t Val) {
	assert(selectOffset(OffR, Val) && "Invalid displacement");
	switch (OffR) {
	case REDEFINEAddressingMode::Off12Only:
		return true;
	}
	llvm_unreachable("Unhandled displacement range");
}

// Return true if Addr is suitable for AM, updating AM if so.
bool REDEFINEDAGToDAGISel::selectAddress(SDValue Addr, REDEFINEAddressingMode &AM) {
	// Start out assuming that the address will need to be loaded separately,
	// then try to extend it as much as we can.
	AM.Base = Addr;

	// First try treating the address as a constant.
	if (Addr.getOpcode() == ISD::Constant && expandOffset(AM, true, SDValue(), cast<ConstantSDNode>(Addr))) {
	}

	// Reject cases where the other instruction in a pair should be used.
	if (!isValidOffset(AM.OffR, AM.Offset))
		return false;

	DEBUG(AM.dump());
	return true;
}

// Insert a node into the DAG at least before Pos.  This will reposition
// the node as needed, and will assign it a node ID that is <= Pos's ID.
// Note that this does *not* preserve the uniqueness of node IDs!
// The selection DAG must no longer depend on their uniqueness when this
// function is used.
static void insertDAGNode(SelectionDAG *DAG, SDNode *Pos, SDValue N) {
	if (N.getNode()->getNodeId() == -1 || N.getNode()->getNodeId() > Pos->getNodeId()) {
		DAG->RepositionNode(Pos, N.getNode());
		N.getNode()->setNodeId(Pos->getNodeId());
	}
}

void REDEFINEDAGToDAGISel::getAddressOperands(const REDEFINEAddressingMode &AM, EVT VT, SDValue &Base, SDValue &Offset) {
	Base = AM.Base;
	if (!Base.getNode())
		// Register 0 means "no base".  This is mostly useful for shifts.
		Base = CurDAG->getRegister(0, VT);
	else if (Base.getOpcode() == ISD::FrameIndex) {
		// Lower a FrameIndex to a TargetFrameIndex.
		int64_t FrameIndex = cast<FrameIndexSDNode>(Base)->getIndex();
		Base = CurDAG->getTargetFrameIndex(FrameIndex, VT);
	} else if (Base.getValueType() != VT) {
		// Truncate values from i64 to i32, for shifts.
		assert(VT == MVT::i32 && Base.getValueType() == MVT::i64 && "Unexpected truncation");
		DebugLoc DL = Base.getDebugLoc();
		SDValue Trunc = CurDAG->getNode(ISD::TRUNCATE, DL, VT, Base);
		insertDAGNode(CurDAG, Base.getNode(), Trunc);
		Base = Trunc;
	}

	// Lower the displacement to a TargetConstant.
	Offset = CurDAG->getTargetConstant(AM.Offset, VT);
}
void REDEFINEDAGToDAGISel::SelectBasicBlock(BasicBlock::const_iterator Begin, BasicBlock::const_iterator End, bool &HadTailCall) {
	// Lower all of the non-terminator instructions. If a call is emitted
	// as a tail call, cease emitting nodes for this block. Terminators
	// are handled below.
	for (BasicBlock::const_iterator I = Begin; I != End && !SDB->HasTailCall; ++I)
		SDB->visit(*I);

	// Make sure the root of the DAG is up-to-date.
	CurDAG->setRoot(SDB->getControlRoot());
	HadTailCall = SDB->HasTailCall;
	SDB->clear();

	// Final step, emit the lowered DAG as machine code.
	CodeGenAndEmitDAG();
}

SDNode *REDEFINEDAGToDAGISel::Select(SDNode *Node) {
	DebugLoc DL = Node->getDebugLoc();
	// Dump information about the Node being selected

	// If we have a custom node, we already have selected!
	if (Node->isMachineOpcode()) {
		DEBUG(errs() << "machine opcode:" << Node->getMachineOpcode() << "\n");
		DEBUG(errs() << "== "; Node->dump(CurDAG); errs() << "\n");
		return 0;
	}

	unsigned Opcode = Node->getOpcode();
	switch (Opcode) {
	case ISD::FrameIndex: {
		//TODO
		//Hack for REDEFINE, add the index object's offset as an immediate value; This might spill into larger than 12 bits, potential problem
		SDValue reg = CurDAG->getRegister(REDEFINE::t5, MVT::getIntegerVT(32));
		SDValue TFI = CurDAG->getTargetFrameIndex(cast<FrameIndexSDNode>(Node)->getIndex(), TLI.getPointerTy());
		unsigned Opc = REDEFINE::ADDI;
		EVT VT = MVT::i32;

		if (Node->hasOneUse()) //don't create a new node just morph this one
			return CurDAG->SelectNodeTo(Node, Opc, VT, reg, TFI);
		return CurDAG->getMachineNode(Opc, DL, VT, reg, TFI);
//		int FI = cast<FrameIndexSDNode>(Node)->getIndex();
//		return CurDAG->getCopyToReg(CurDAG->getRoot(),DL ,FI, MVT::i32).getNode();
	}
	}	//end special selections

	// Select the default instruction
	SDNode *ResNode = SelectCode(Node);

	DEBUG(errs() << "=> "; if (ResNode == NULL || ResNode == Node) Node->dump(CurDAG); else ResNode->dump(CurDAG); errs() << "\n";);
	return ResNode;
}

bool REDEFINEDAGToDAGISel::SelectInlineAsmMemoryOperand(const SDValue &Op, char ConstraintCode, std::vector<SDValue> &OutOps) {
	assert(ConstraintCode == 'm' && "Unexpected constraint code");

	SDValue Base, Offset;
	selectMemRegAddr(Op, Base, Offset);
	OutOps.push_back(Base);
	OutOps.push_back(Offset);
	return false;
}

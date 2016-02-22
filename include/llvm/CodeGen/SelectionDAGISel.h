//===-- llvm/CodeGen/SelectionDAGISel.h - Common Base Class------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the SelectionDAGISel class, which is used as the common
// base class for SelectionDAG-based instruction selectors.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CODEGEN_SELECTIONDAGISEL_H
#define LLVM_CODEGEN_SELECTIONDAGISEL_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "FunctionLoweringInfo.h"
#include "MachineFunctionPass.h"
#include "SelectionDAG.h"
#include "SelectionDAGNodes.h"
using namespace llvm;

STATISTIC(NumFastIselFailures, "Number of instructions fast isel failed on");
STATISTIC(NumFastIselSuccess, "Number of instructions fast isel selected");
STATISTIC(NumFastIselBlocks, "Number of blocks selected entirely by fast isel");
STATISTIC(NumDAGBlocks, "Number of blocks selected using DAG");
STATISTIC(NumDAGIselRetries, "Number of times dag isel has to try another path");
STATISTIC(NumEntryBlocks, "Number of entry blocks encountered");
STATISTIC(NumFastIselFailLowerArguments, "Number of entry blocks where fast isel failed to lower arguments");

// Terminators
STATISTIC(NumFastIselFailRet, "Fast isel fails on Ret");
STATISTIC(NumFastIselFailBr, "Fast isel fails on Br");
STATISTIC(NumFastIselFailSwitch, "Fast isel fails on Switch");
STATISTIC(NumFastIselFailIndirectBr, "Fast isel fails on IndirectBr");
STATISTIC(NumFastIselFailInvoke, "Fast isel fails on Invoke");
STATISTIC(NumFastIselFailResume, "Fast isel fails on Resume");
STATISTIC(NumFastIselFailUnreachable, "Fast isel fails on Unreachable");

// Standard binary operators...
STATISTIC(NumFastIselFailAdd, "Fast isel fails on Add");
STATISTIC(NumFastIselFailFAdd, "Fast isel fails on FAdd");
STATISTIC(NumFastIselFailSub, "Fast isel fails on Sub");
STATISTIC(NumFastIselFailFSub, "Fast isel fails on FSub");
STATISTIC(NumFastIselFailMul, "Fast isel fails on Mul");
STATISTIC(NumFastIselFailFMul, "Fast isel fails on FMul");
STATISTIC(NumFastIselFailUDiv, "Fast isel fails on UDiv");
STATISTIC(NumFastIselFailSDiv, "Fast isel fails on SDiv");
STATISTIC(NumFastIselFailFDiv, "Fast isel fails on FDiv");
STATISTIC(NumFastIselFailURem, "Fast isel fails on URem");
STATISTIC(NumFastIselFailSRem, "Fast isel fails on SRem");
STATISTIC(NumFastIselFailFRem, "Fast isel fails on FRem");

// Logical operators...
STATISTIC(NumFastIselFailAnd, "Fast isel fails on And");
STATISTIC(NumFastIselFailOr, "Fast isel fails on Or");
STATISTIC(NumFastIselFailXor, "Fast isel fails on Xor");

// Memory instructions...
STATISTIC(NumFastIselFailAlloca, "Fast isel fails on Alloca");
STATISTIC(NumFastIselFailLoad, "Fast isel fails on Load");
STATISTIC(NumFastIselFailStore, "Fast isel fails on Store");
STATISTIC(NumFastIselFailAtomicCmpXchg, "Fast isel fails on AtomicCmpXchg");
STATISTIC(NumFastIselFailAtomicRMW, "Fast isel fails on AtomicRWM");
STATISTIC(NumFastIselFailFence, "Fast isel fails on Frence");
STATISTIC(NumFastIselFailGetElementPtr, "Fast isel fails on GetElementPtr");

// Convert instructions...
STATISTIC(NumFastIselFailTrunc, "Fast isel fails on Trunc");
STATISTIC(NumFastIselFailZExt, "Fast isel fails on ZExt");
STATISTIC(NumFastIselFailSExt, "Fast isel fails on SExt");
STATISTIC(NumFastIselFailFPTrunc, "Fast isel fails on FPTrunc");
STATISTIC(NumFastIselFailFPExt, "Fast isel fails on FPExt");
STATISTIC(NumFastIselFailFPToUI, "Fast isel fails on FPToUI");
STATISTIC(NumFastIselFailFPToSI, "Fast isel fails on FPToSI");
STATISTIC(NumFastIselFailUIToFP, "Fast isel fails on UIToFP");
STATISTIC(NumFastIselFailSIToFP, "Fast isel fails on SIToFP");
STATISTIC(NumFastIselFailIntToPtr, "Fast isel fails on IntToPtr");
STATISTIC(NumFastIselFailPtrToInt, "Fast isel fails on PtrToInt");
STATISTIC(NumFastIselFailBitCast, "Fast isel fails on BitCast");

// Other instructions...
STATISTIC(NumFastIselFailICmp, "Fast isel fails on ICmp");
STATISTIC(NumFastIselFailFCmp, "Fast isel fails on FCmp");
STATISTIC(NumFastIselFailPHI, "Fast isel fails on PHI");
STATISTIC(NumFastIselFailSelect, "Fast isel fails on Select");
STATISTIC(NumFastIselFailCall, "Fast isel fails on Call");
STATISTIC(NumFastIselFailShl, "Fast isel fails on Shl");
STATISTIC(NumFastIselFailLShr, "Fast isel fails on LShr");
STATISTIC(NumFastIselFailAShr, "Fast isel fails on AShr");
STATISTIC(NumFastIselFailVAArg, "Fast isel fails on VAArg");
STATISTIC(NumFastIselFailExtractElement, "Fast isel fails on ExtractElement");
STATISTIC(NumFastIselFailInsertElement, "Fast isel fails on InsertElement");
STATISTIC(NumFastIselFailShuffleVector, "Fast isel fails on ShuffleVector");
STATISTIC(NumFastIselFailExtractValue, "Fast isel fails on ExtractValue");
STATISTIC(NumFastIselFailInsertValue, "Fast isel fails on InsertValue");
STATISTIC(NumFastIselFailLandingPad, "Fast isel fails on LandingPad");

#ifndef NDEBUG
extern cl::opt<bool> EnableFastISelVerbose2;
#endif

extern cl::opt<bool> EnableFastISelVerbose;
extern cl::opt<bool> EnableFastISelAbort;
extern cl::opt<bool> EnableFastISelAbortArgs;

extern cl::opt<bool> UseMBPI;

#ifndef NDEBUG
extern cl::opt<bool> ViewDAGCombine1;
extern cl::opt<bool> ViewLegalizeTypesDAGs;
extern cl::opt<bool> ViewLegalizeDAGs;
extern cl::opt<bool> ViewDAGCombine2;
extern cl::opt<bool> ViewDAGCombineLT;
extern cl::opt<bool> ViewISelDAGs;
extern cl::opt<bool> ViewSchedDAGs;
extern cl::opt<bool> ViewSUnitDAGs;
#else
extern const bool ViewDAGCombine1 = false,
ViewLegalizeTypesDAGs = false, ViewLegalizeDAGs = false,
ViewDAGCombine2 = false,
ViewDAGCombineLT = false,
ViewISelDAGs = false, ViewSchedDAGs = false,
ViewSUnitDAGs = false;
#endif

namespace llvm {
class FastISel;
class SelectionDAGBuilder;
class SDValue;
class MachineRegisterInfo;
class MachineBasicBlock;
class MachineFunction;
class MachineInstr;
class TargetLowering;
class TargetLibraryInfo;
class TargetInstrInfo;
class TargetTransformInfo;
class FunctionLoweringInfo;
class ScheduleHazardRecognizer;
class GCFunctionInfo;
class ScheduleDAGSDNodes;
class LoadInst;

/// SelectionDAGISel - This is the common base class used for SelectionDAG-based
/// pattern-matching instruction selectors.
class SelectionDAGISel: public MachineFunctionPass {
public:
	const TargetMachine &TM;
	const TargetLowering &TLI;
	const TargetLibraryInfo *LibInfo;
	const TargetTransformInfo *TTI;
	FunctionLoweringInfo *FuncInfo;
	MachineFunction *MF;
	MachineRegisterInfo *RegInfo;
	SelectionDAG *CurDAG;
	SelectionDAGBuilder *SDB;
	AliasAnalysis *AA;
	GCFunctionInfo *GFI;
	CodeGenOpt::Level OptLevel;
	static char ID;

	explicit SelectionDAGISel(const TargetMachine &tm, CodeGenOpt::Level OL = CodeGenOpt::Default);
	virtual ~SelectionDAGISel();

	const TargetLowering &getTargetLowering() {
		return TLI;
	}

	virtual void getAnalysisUsage(AnalysisUsage &AU) const;

	virtual bool runOnMachineFunction(MachineFunction &MF);

	virtual void EmitFunctionEntryCode() {
	}

	/// PreprocessISelDAG - This hook allows targets to hack on the graph before
	/// instruction selection starts.
	virtual void PreprocessISelDAG() {
	}

	/// PostprocessISelDAG() - This hook allows the target to hack on the graph
	/// right after selection.
	virtual void PostprocessISelDAG() {
	}

	/// Select - Main hook targets implement to select a node.
	virtual SDNode *Select(SDNode *N) = 0;

	/// SelectInlineAsmMemoryOperand - Select the specified address as a target
	/// addressing mode, according to the specified constraint code.  If this does
	/// not match or is not implemented, return true.  The resultant operands
	/// (which will appear in the machine instruction) should be added to the
	/// OutOps vector.
	virtual bool SelectInlineAsmMemoryOperand(const SDValue &Op, char ConstraintCode, std::vector<SDValue> &OutOps) {
		return true;
	}

	/// IsProfitableToFold - Returns true if it's profitable to fold the specific
	/// operand node N of U during instruction selection that starts at Root.
	virtual bool IsProfitableToFold(SDValue N, SDNode *U, SDNode *Root) const;

	/// IsLegalToFold - Returns true if the specific operand node N of
	/// U can be folded during instruction selection that starts at Root.
	/// FIXME: This is a static member function because the MSP430/X86
	/// targets, which uses it during isel.  This could become a proper member.
	static bool IsLegalToFold(SDValue N, SDNode *U, SDNode *Root, CodeGenOpt::Level OptLevel, bool IgnoreChains = false);

	void checkCLFlags();
	static void SplitCriticalSideEffectEdges(Function &Fn, Pass *SDISel);

	// Opcodes used by the DAG state machine:
	enum BuiltinOpcodes {
		OPC_Scope,
		OPC_RecordNode,
		OPC_RecordChild0,
		OPC_RecordChild1,
		OPC_RecordChild2,
		OPC_RecordChild3,
		OPC_RecordChild4,
		OPC_RecordChild5,
		OPC_RecordChild6,
		OPC_RecordChild7,
		OPC_RecordMemRef,
		OPC_CaptureGlueInput,
		OPC_MoveChild,
		OPC_MoveParent,
		OPC_CheckSame,
		OPC_CheckPatternPredicate,
		OPC_CheckPredicate,
		OPC_CheckOpcode,
		OPC_SwitchOpcode,
		OPC_CheckType,
		OPC_SwitchType,
		OPC_CheckChild0Type,
		OPC_CheckChild1Type,
		OPC_CheckChild2Type,
		OPC_CheckChild3Type,
		OPC_CheckChild4Type,
		OPC_CheckChild5Type,
		OPC_CheckChild6Type,
		OPC_CheckChild7Type,
		OPC_CheckInteger,
		OPC_CheckCondCode,
		OPC_CheckValueType,
		OPC_CheckComplexPat,
		OPC_CheckAndImm,
		OPC_CheckOrImm,
		OPC_CheckFoldableChainNode,

		OPC_EmitInteger,
		OPC_EmitRegister,
		OPC_EmitRegister2,
		OPC_EmitConvertToTarget,
		OPC_EmitMergeInputChains,
		OPC_EmitMergeInputChains1_0,
		OPC_EmitMergeInputChains1_1,
		OPC_EmitCopyToReg,
		OPC_EmitNodeXForm,
		OPC_EmitNode,
		OPC_MorphNodeTo,
		OPC_MarkGlueResults,
		OPC_CompleteMatch
	};

	enum {
		OPFL_None = 0,  // Node has no chain or glue input and isn't variadic.
		OPFL_Chain = 1,     // Node has a chain input.
		OPFL_GlueInput = 2,     // Node has a glue input.
		OPFL_GlueOutput = 4,     // Node has a glue output.
		OPFL_MemRefs = 8,     // Node gets accumulated MemRefs.
		OPFL_Variadic0 = 1 << 4,  // Node is variadic, root has 0 fixed inputs.
		OPFL_Variadic1 = 2 << 4,  // Node is variadic, root has 1 fixed inputs.
		OPFL_Variadic2 = 3 << 4,  // Node is variadic, root has 2 fixed inputs.
		OPFL_Variadic3 = 4 << 4,  // Node is variadic, root has 3 fixed inputs.
		OPFL_Variadic4 = 5 << 4,  // Node is variadic, root has 4 fixed inputs.
		OPFL_Variadic5 = 6 << 4,  // Node is variadic, root has 5 fixed inputs.
		OPFL_Variadic6 = 7 << 4,  // Node is variadic, root has 6 fixed inputs.

		OPFL_VariadicInfo = OPFL_Variadic6
	};

	/// getNumFixedFromVariadicInfo - Transform an EmitNode flags word into the
	/// number of fixed arity values that should be skipped when copying from the
	/// root.
	static inline int getNumFixedFromVariadicInfo(unsigned Flags) {
		return ((Flags & OPFL_VariadicInfo) >> 4) - 1;
	}

protected:
	/// DAGSize - Size of DAG being instruction selected.
	///
	unsigned DAGSize;

	/// ReplaceUses - replace all uses of the old node F with the use
	/// of the new node T.
	void ReplaceUses(SDValue F, SDValue T) {
		CurDAG->ReplaceAllUsesOfValueWith(F, T);
	}

	/// ReplaceUses - replace all uses of the old nodes F with the use
	/// of the new nodes T.
	void ReplaceUses(const SDValue *F, const SDValue *T, unsigned Num) {
		CurDAG->ReplaceAllUsesOfValuesWith(F, T, Num);
	}

	/// ReplaceUses - replace all uses of the old node F with the use
	/// of the new node T.
	void ReplaceUses(SDNode *F, SDNode *T) {
		CurDAG->ReplaceAllUsesWith(F, T);
	}

	/// SelectInlineAsmMemoryOperands - Calls to this are automatically generated
	/// by tblgen.  Others should not call it.
	void SelectInlineAsmMemoryOperands(std::vector<SDValue> &Ops);

public:
	// Calls to these predicates are generated by tblgen.
	bool CheckAndMask(SDValue LHS, ConstantSDNode *RHS, int64_t DesiredMaskS) const;
	bool CheckOrMask(SDValue LHS, ConstantSDNode *RHS, int64_t DesiredMaskS) const;

	/// CheckPatternPredicate - This function is generated by tblgen in the
	/// target.  It runs the specified pattern predicate and returns true if it
	/// succeeds or false if it fails.  The number is a private implementation
	/// detail to the code tblgen produces.
	virtual bool CheckPatternPredicate(unsigned PredNo) const {
		llvm_unreachable("Tblgen should generate the implementation of this!");
	}

	/// CheckNodePredicate - This function is generated by tblgen in the target.
	/// It runs node predicate number PredNo and returns true if it succeeds or
	/// false if it fails.  The number is a private implementation
	/// detail to the code tblgen produces.
	virtual bool CheckNodePredicate(SDNode *N, unsigned PredNo) const {
		llvm_unreachable("Tblgen should generate the implementation of this!");
	}

	virtual bool CheckComplexPattern(SDNode *Root, SDNode *Parent, SDValue N, unsigned PatternNo, SmallVectorImpl<std::pair<SDValue, SDNode*> > &Result) {
		llvm_unreachable("Tblgen should generate the implementation of this!");
	}

	virtual SDValue RunSDNodeXForm(SDValue V, unsigned XFormNo) {
		llvm_unreachable("Tblgen should generate this!");
	}

	SDNode *SelectCodeCommon(SDNode *NodeToMatch, const unsigned char *MatcherTable, unsigned TableSize);

private:

	// Calls to these functions are generated by tblgen.
	SDNode *Select_INLINEASM(SDNode *N);
	SDNode *Select_UNDEF(SDNode *N);
	void CannotYetSelect(SDNode *N);

protected:
	//Made public for REDEFINE to inherit this method
	/// \brief Perform instruction selection on all basic blocks in the function.
	virtual void SelectAllBasicBlocks(const Function &Fn);

	/// \brief Perform instruction selection on a single basic block, for
	/// instructions between \p Begin and \p End.  \p HadTailCall will be set
	/// to true if a call in the block was translated as a tail call.
	virtual void SelectBasicBlock(BasicBlock::const_iterator Begin, BasicBlock::const_iterator End, bool &HadTailCall);

	virtual void CodeGenAndEmitDAG();

	virtual void FinishBasicBlock();

	/// Create the scheduler. If a specific scheduler was specified
	/// via the SchedulerRegistry, use it, otherwise select the
	/// one preferred by the target.
	///
	ScheduleDAGSDNodes *CreateScheduler();

	/// \brief Generate instructions for lowering the incoming arguments of the
		/// given function.

	void ComputeLiveOutVRegInfo();

	void DoInstructionSelection();


private:
	void LowerArguments(const Function &F);
	SDNode *MorphNode(SDNode *Node, unsigned TargetOpc, SDVTList VTs, const SDValue *Ops, unsigned NumOps, unsigned EmitNodeInfo);

	void PrepareEHLandingPad();



	/// OpcodeOffset - This is a cache used to dispatch efficiently into isel
	/// state machines that start with a OPC_SwitchOpcode node.
	std::vector<unsigned> OpcodeOffset;

	void UpdateChainsAndGlue(SDNode *NodeToMatch, SDValue InputChain, const SmallVectorImpl<SDNode*> &ChainNodesMatched, SDValue InputGlue, const SmallVectorImpl<SDNode*> &F, bool isMorphNodeTo);

};

/// isFoldedOrDeadInstruction - Return true if the specified instruction is
/// side-effect free and is either dead or folded into a generated instruction.
/// Return false if it needs to be emitted.
static bool isFoldedOrDeadInstruction(const Instruction *I, FunctionLoweringInfo *FuncInfo) {
	return !I->mayWriteToMemory() && // Side-effecting instructions aren't folded.
			!isa<TerminatorInst>(I) && // Terminators aren't folded.
			!isa<DbgInfoIntrinsic>(I) &&  // Debug instructions aren't folded.
			!isa<LandingPadInst>(I) &&    // Landingpad instructions aren't folded.
			!FuncInfo->isExportedInst(I); // Exported instrs must be computed.
}

#ifndef NDEBUG
// Collect per Instruction statistics for fast-isel misses.  Only those
// instructions that cause the bail are accounted for.  It does not account for
// instructions higher in the block.  Thus, summing the per instructions stats
// will not add up to what is reported by NumFastIselFailures.
static void collectFailStats(const Instruction *I) {
	switch (I->getOpcode()) {
	default:
		assert(0 && "<Invalid operator> ");

		// Terminators
	case Instruction::Ret:
		NumFastIselFailRet++;
		return;
	case Instruction::Br:
		NumFastIselFailBr++;
		return;
	case Instruction::Switch:
		NumFastIselFailSwitch++;
		return;
	case Instruction::IndirectBr:
		NumFastIselFailIndirectBr++;
		return;
	case Instruction::Invoke:
		NumFastIselFailInvoke++;
		return;
	case Instruction::Resume:
		NumFastIselFailResume++;
		return;
	case Instruction::Unreachable:
		NumFastIselFailUnreachable++;
		return;

		// Standard binary operators...
	case Instruction::Add:
		NumFastIselFailAdd++;
		return;
	case Instruction::FAdd:
		NumFastIselFailFAdd++;
		return;
	case Instruction::Sub:
		NumFastIselFailSub++;
		return;
	case Instruction::FSub:
		NumFastIselFailFSub++;
		return;
	case Instruction::Mul:
		NumFastIselFailMul++;
		return;
	case Instruction::FMul:
		NumFastIselFailFMul++;
		return;
	case Instruction::UDiv:
		NumFastIselFailUDiv++;
		return;
	case Instruction::SDiv:
		NumFastIselFailSDiv++;
		return;
	case Instruction::FDiv:
		NumFastIselFailFDiv++;
		return;
	case Instruction::URem:
		NumFastIselFailURem++;
		return;
	case Instruction::SRem:
		NumFastIselFailSRem++;
		return;
	case Instruction::FRem:
		NumFastIselFailFRem++;
		return;

		// Logical operators...
	case Instruction::And:
		NumFastIselFailAnd++;
		return;
	case Instruction::Or:
		NumFastIselFailOr++;
		return;
	case Instruction::Xor:
		NumFastIselFailXor++;
		return;

		// Memory instructions...
	case Instruction::Alloca:
		NumFastIselFailAlloca++;
		return;
	case Instruction::Load:
		NumFastIselFailLoad++;
		return;
	case Instruction::Store:
		NumFastIselFailStore++;
		return;
	case Instruction::AtomicCmpXchg:
		NumFastIselFailAtomicCmpXchg++;
		return;
	case Instruction::AtomicRMW:
		NumFastIselFailAtomicRMW++;
		return;
	case Instruction::Fence:
		NumFastIselFailFence++;
		return;
	case Instruction::GetElementPtr:
		NumFastIselFailGetElementPtr++;
		return;

		// Convert instructions...
	case Instruction::Trunc:
		NumFastIselFailTrunc++;
		return;
	case Instruction::ZExt:
		NumFastIselFailZExt++;
		return;
	case Instruction::SExt:
		NumFastIselFailSExt++;
		return;
	case Instruction::FPTrunc:
		NumFastIselFailFPTrunc++;
		return;
	case Instruction::FPExt:
		NumFastIselFailFPExt++;
		return;
	case Instruction::FPToUI:
		NumFastIselFailFPToUI++;
		return;
	case Instruction::FPToSI:
		NumFastIselFailFPToSI++;
		return;
	case Instruction::UIToFP:
		NumFastIselFailUIToFP++;
		return;
	case Instruction::SIToFP:
		NumFastIselFailSIToFP++;
		return;
	case Instruction::IntToPtr:
		NumFastIselFailIntToPtr++;
		return;
	case Instruction::PtrToInt:
		NumFastIselFailPtrToInt++;
		return;
	case Instruction::BitCast:
		NumFastIselFailBitCast++;
		return;

		// Other instructions...
	case Instruction::ICmp:
		NumFastIselFailICmp++;
		return;
	case Instruction::FCmp:
		NumFastIselFailFCmp++;
		return;
	case Instruction::PHI:
		NumFastIselFailPHI++;
		return;
	case Instruction::Select:
		NumFastIselFailSelect++;
		return;
	case Instruction::Call:
		NumFastIselFailCall++;
		return;
	case Instruction::Shl:
		NumFastIselFailShl++;
		return;
	case Instruction::LShr:
		NumFastIselFailLShr++;
		return;
	case Instruction::AShr:
		NumFastIselFailAShr++;
		return;
	case Instruction::VAArg:
		NumFastIselFailVAArg++;
		return;
	case Instruction::ExtractElement:
		NumFastIselFailExtractElement++;
		return;
	case Instruction::InsertElement:
		NumFastIselFailInsertElement++;
		return;
	case Instruction::ShuffleVector:
		NumFastIselFailShuffleVector++;
		return;
	case Instruction::ExtractValue:
		NumFastIselFailExtractValue++;
		return;
	case Instruction::InsertValue:
		NumFastIselFailInsertValue++;
		return;
	case Instruction::LandingPad:
		NumFastIselFailLandingPad++;
		return;
	}
}
#endif

}

#endif /* LLVM_CODEGEN_SELECTIONDAGISEL_H */

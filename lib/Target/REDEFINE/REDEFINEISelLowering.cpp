//===-- REDEFINEISelLowering.cpp - REDEFINE DAG lowering implementation -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the REDEFINETargetLowering class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "REDEFINE-lower"

#include "REDEFINEISelLowering.h"
#include "REDEFINECallingConv.h"
#include "REDEFINEConstantPoolValue.h"
#include "REDEFINETargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static const uint16_t REDEFINE32IntRegs[8] = { REDEFINE::a0, REDEFINE::a1, REDEFINE::a2, REDEFINE::a3, REDEFINE::a4, REDEFINE::a5, REDEFINE::a6, REDEFINE::a7 };

//static const uint16_t FPFRegs[8] = { REDEFINE::fa0, REDEFINE::fa1, REDEFINE::fa2, REDEFINE::fa3, REDEFINE::fa4, REDEFINE::fa5, REDEFINE::fa6, REDEFINE::fa7 };

REDEFINETargetLowering::REDEFINETargetLowering(REDEFINETargetMachine &tm) :
		TargetLowering(tm, new TargetLoweringObjectFileELF()), Subtarget(*tm.getSubtargetImpl()), TM(tm), IsREDEFINE32(Subtarget.isREDEFINE32()) {
	MVT PtrVT = getPointerTy();

	// Set up the register classes.
	addRegisterClass(MVT::i1, &REDEFINE::GR32BitRegClass);
	addRegisterClass(MVT::i8, &REDEFINE::GR32BitRegClass);
	addRegisterClass(MVT::i16, &REDEFINE::GR32BitRegClass);
	addRegisterClass(MVT::i32, &REDEFINE::GR32BitRegClass);
	addRegisterClass(MVT::f16,  &REDEFINE::FP32BitRegClass);
    addRegisterClass(MVT::f32,  &REDEFINE::FP32BitRegClass);


	setSchedulingPreference(Sched::ILP);

	//For i1 types all bits are zero except bit 0
	setBooleanContents(ZeroOrOneBooleanContent);
	setBooleanVectorContents(ZeroOrOneBooleanContent); //vectors of i1s are the same

	// Used by legalize types to correctly generate the setcc result.
	AddPromotedToType(ISD::SETCC, MVT::i1, MVT::i32);

	// Instructions are strings of 2-byte aligned 2-byte values.
	// align by log2(2) bytes?
	setMinFunctionAlignment(2);

	// Handle operations that are handled in a similar way for all types.
	for (unsigned I = MVT::FIRST_INTEGER_VALUETYPE; I <= MVT::LAST_FP_VALUETYPE; ++I) {
		MVT VT = MVT::SimpleValueType(I);
		if (isTypeLegal(VT)) {
			// Lower SELECT_CC and BR_CC into separate comparisons and branches.
			setOperationAction(ISD::SELECT_CC, VT, Expand);
			setOperationAction(ISD::BR_CC, VT, Expand);

		}
	}

	setOperationAction(ISD::SETCC, MVT::i32, Legal);		  //folds into brcond
	setOperationAction(ISD::SETCC, MVT::i64, Expand);		  //only use 32bit
	setOperationAction(ISD::Constant, MVT::i32, Legal);
	setOperationAction(ISD::ConstantFP, MVT::i32, Legal);
	setOperationAction(ISD::Constant, MVT::i64, Expand);

	// Expand jump table branches as address arithmetic followed by an
	// indirect jump.
	setOperationAction(ISD::BR_JT, MVT::Other, Expand);
	//RISCV also does not have indirect branch so expand them
	//TODO: don't we have one via JALR?
	setOperationAction(ISD::BRIND, MVT::Other, Expand);

	//make BRCOND legal, its actually only legal for a subset of conds
	setOperationAction(ISD::BRCOND, MVT::Other, Legal);

	//Custom Lower Overflow operators

	// Handle integer types.
	for (unsigned I = MVT::FIRST_INTEGER_VALUETYPE; I <= MVT::LAST_INTEGER_VALUETYPE; ++I) {
		MVT VT = MVT::SimpleValueType(I);
		if (isTypeLegal(VT)) {
			setOperationAction(ISD::MUL, VT, Legal);
			setOperationAction(ISD::MULHS, VT, Legal);
			setOperationAction(ISD::MULHU, VT, Legal);
			setOperationAction(ISD::SDIV, VT, Legal);
			setOperationAction(ISD::UDIV, VT, Legal);
			setOperationAction(ISD::SREM, VT, Legal);
			setOperationAction(ISD::UREM, VT, Legal);
			//No support at all
			setOperationAction(ISD::SDIVREM, VT, Expand);
			setOperationAction(ISD::UDIVREM, VT, Expand);
			//RISCV doesn't support  [ADD,SUB][E,C]
			setOperationAction(ISD::ADDE, VT, Expand);
			setOperationAction(ISD::SUBE, VT, Expand);
			setOperationAction(ISD::ADDC, VT, Expand);
			setOperationAction(ISD::SUBC, VT, Expand);
			//RISCV doesn't support s[hl,rl,ra]_parts
			setOperationAction(ISD::SHL_PARTS, VT, Expand);
			setOperationAction(ISD::SRL_PARTS, VT, Expand);
			setOperationAction(ISD::SRA_PARTS, VT, Expand);
			//RISCV doesn't support rotl
			setOperationAction(ISD::ROTL, VT, Expand);
			setOperationAction(ISD::ROTR, VT, Expand);

			// Expand ATOMIC_LOAD and ATOMIC_STORE using ATOMIC_CMP_SWAP.
//			// FIXME: probably much too conservative.
			//TODO All atomic operations commented out for now
//			setOperationAction(ISD::ATOMIC_LOAD, VT, Expand);
//			setOperationAction(ISD::ATOMIC_STORE, VT, Expand);

			// No special instructions for these.
			setOperationAction(ISD::CTPOP, VT, Expand);
			setOperationAction(ISD::CTTZ, VT, Expand);
			setOperationAction(ISD::CTLZ, VT, Expand);
			setOperationAction(ISD::CTTZ_ZERO_UNDEF, VT, Expand);
			setOperationAction(ISD::CTLZ_ZERO_UNDEF, VT, Expand);

		}
	}

	//to have the best chance and doing something good with fences custom lower them
//	setOperationAction(ISD::ATOMIC_FENCE, MVT::Other, Legal);
	//Some Atmoic ops are legal
	//No atomic ops so expand all
//	setOperationAction(ISD::ATOMIC_SWAP, MVT::i32, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_ADD, MVT::i32, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_AND, MVT::i32, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_OR, MVT::i32, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_XOR, MVT::i32, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_MIN, MVT::i32, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_MAX, MVT::i32, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_UMIN, MVT::i32, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_UMAX, MVT::i32, Expand);
//	setOperationAction(ISD::ATOMIC_SWAP, MVT::i64, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_ADD, MVT::i64, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_AND, MVT::i64, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_OR, MVT::i64, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_XOR, MVT::i64, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_MIN, MVT::i64, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_MAX, MVT::i64, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_UMIN, MVT::i64, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_UMAX, MVT::i64, Expand);
//	setOperationAction(ISD::ATOMIC_CMP_SWAP, MVT::i32, Expand);
//	setOperationAction(ISD::ATOMIC_CMP_SWAP, MVT::i64, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_NAND, MVT::i32, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_SUB, MVT::i32, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_NAND, MVT::i64, Expand);
//	setOperationAction(ISD::ATOMIC_LOAD_SUB, MVT::i64, Expand);

	setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
	setOperationAction(ISD::SMUL_LOHI, MVT::i64, Expand);
	setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);
	setOperationAction(ISD::UMUL_LOHI, MVT::i64, Expand);

	// No sign extend instructions
	setLoadExtAction(ISD::SEXTLOAD, MVT::i1, Expand);
	setLoadExtAction(ISD::ZEXTLOAD, MVT::i1, Expand);
	setLoadExtAction(ISD::EXTLOAD, MVT::i1, Expand);
	setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1, Expand);
	setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i8, Expand);
	setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i16, Expand);
	setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i32, Expand);

	// Handle the various types of symbolic address.
	setOperationAction(ISD::ConstantPool, PtrVT, Custom);
	setOperationAction(ISD::GlobalAddress, PtrVT, Custom);
	setOperationAction(ISD::GlobalTLSAddress, PtrVT, Custom);
	setOperationAction(ISD::BlockAddress, PtrVT, Custom);
	setOperationAction(ISD::JumpTable, PtrVT, Custom);

	//Expand stack allocations
//		  setOperationAction(ISD::DYNAMIC_STACKALLOC, PtrVT, Expand);
//
//		  // Use custom expanders so that we can force the function to use
//		  // a frame pointer.
//		  // TODO: real comment
//		  setOperationAction(ISD::STACKSAVE,    MVT::Other, Expand);
//		  setOperationAction(ISD::STACKRESTORE, MVT::Other, Expand);
//		  setOperationAction(ISD::FRAMEADDR,    MVT::Other, Expand);
//
//		  // Expand these using getExceptionSelectorRegister() and
//		  // getExceptionPointerRegister().
//		  setOperationAction(ISD::EXCEPTIONADDR, PtrVT, Expand);
//		  setOperationAction(ISD::EHSELECTION,   PtrVT, Expand);

	// Handle floating-point types.
	for (unsigned I = MVT::FIRST_FP_VALUETYPE; I <= MVT::LAST_FP_VALUETYPE; ++I) {
		MVT VT = MVT::SimpleValueType(I);
		if (isTypeLegal(VT)) {
			// We can use FI for FRINT.
			//setOperationAction(ISD::FRINT, VT, Legal);
			setOperationAction(ISD::FADD, VT, Legal);
			setOperationAction(ISD::FSUB, VT, Legal);
			setOperationAction(ISD::FMUL, VT, Legal);
			setOperationAction(ISD::FDIV, VT, Legal);
			setOperationAction(ISD::FSQRT, VT, Legal);
			// No special instructions for these.
			setOperationAction(ISD::FSIN, VT, Legal);
			setOperationAction(ISD::FCOS, VT, Legal);
			setOperationAction(ISD::FREM, VT, Expand);
			setOperationAction(ISD::FABS, VT, Expand);
			setOperationAction(ISD::FMINNUM, VT, Legal);
			setOperationAction(ISD::FMAXNUM, VT, Legal);
		}
	}

	// Handle floating-point types.
	setOperationAction(ISD::FMA, MVT::f32, Legal);
	setOperationAction(ISD::BITCAST, MVT::i32, Legal);
	setOperationAction(ISD::BITCAST, MVT::f32, Legal);
	setOperationAction(ISD::UINT_TO_FP, MVT::i32, Legal);
	setOperationAction(ISD::SINT_TO_FP, MVT::i32, Legal);
	setOperationAction(ISD::FP_TO_UINT, MVT::i32, Legal);
	setOperationAction(ISD::FP_TO_SINT, MVT::i32, Legal);
	setOperationAction(ISD::FCOPYSIGN, MVT::f32, Legal);
	setOperationAction(ISD::FP_ROUND, MVT::f32, Legal);
	setOperationAction(ISD::FP_ROUND, MVT::f32, Legal);


	// Needed so that we don't try to implement f128 constant loads using
	// a load-and-extend of a f80 constant (in cases where the constant
	// would fit in an f80).
//	setLoadExtAction(ISD::EXTLOAD, MVT::f80, Expand);

	// Floating-point truncation and stores need to be done separately.
//	setTruncStoreAction(MVT::f64, MVT::f32, Expand);
//	setTruncStoreAction(MVT::f128, MVT::f32, Expand);
//	setTruncStoreAction(MVT::f128, MVT::f64, Expand);

	// We have 64-bit FPR<->GPR moves, but need special handling for
	// 32-bit forms.

	// VASTART and VACOPY need to deal with the RISCV-specific varargs
	// structure, but VAEND is a no-op.
//		  setOperationAction(ISD::VASTART, MVT::Other, Expand);
//		  //we always write var args with word boundary so we have to customize this
//		  setOperationAction(ISD::VAARG  , MVT::Other, Custom);
//		  setOperationAction(ISD::VACOPY , MVT::Other, Expand);
//		  setOperationAction(ISD::VAEND  , MVT::Other, Expand);

	// Compute derived properties from the register classes
	computeRegisterProperties();

	// Division is inexpensive
	setIntDivIsCheap(true);
}

bool REDEFINETargetLowering::isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const {
	// The REDEFINE target isn't yet aware of offsets.
	return false;
}

bool REDEFINETargetLowering::isFPImmLegal(const APFloat &Imm, EVT VT) const {
	// We can load zero using LZ?R and negative zero using LZ?R;LC?BR.
	return Imm.isPosZero();
}

//===----------------------------------------------------------------------===//
// Inline asm support
//===----------------------------------------------------------------------===//

TargetLowering::ConstraintType REDEFINETargetLowering::getConstraintType(const std::string &Constraint) const {
	if (Constraint.size() == 1) {
		switch (Constraint[0]) {
			case 'a': // Address register
			case 'd': // Data register (equivalent to 'r')
			case 'f': // Floating-point register
			case 'r': // General-purpose register
				return C_RegisterClass;

			case 'Q': // Memory with base and unsigned 12-bit displacement
			case 'R': // Likewise, plus an index
			case 'S': // Memory with base and signed 20-bit displacement
			case 'T': // Likewise, plus an index
			case 'm': // Equivalent to 'T'.
				return C_Memory;

			case 'I': // Unsigned 8-bit constant
			case 'J': // Unsigned 12-bit constant
			case 'K': // Signed 16-bit constant
			case 'L': // Signed 20-bit displacement (on all targets we support)
			case 'M': // 0x7fffffff
				return C_Other;

			default:
				break;
		}
	}
	return TargetLowering::getConstraintType(Constraint);
}

TargetLowering::ConstraintWeight REDEFINETargetLowering::getSingleConstraintMatchWeight(AsmOperandInfo &info, const char *constraint) const {
	ConstraintWeight weight = CW_Invalid;
	Value *CallOperandVal = info.CallOperandVal;
	// If we don't have a value, we can't do a match,
	// but allow it at the lowest weight.
	if (CallOperandVal == NULL) return CW_Default;
	Type *type = CallOperandVal->getType();
	// Look at the constraint type.
	switch (*constraint) {
		default:
			weight = TargetLowering::getSingleConstraintMatchWeight(info, constraint);
			break;

		case 'a': // Address register
		case 'd': // Data register (equivalent to 'r')
		case 'r': // General-purpose register
			if (CallOperandVal->getType()->isIntegerTy()) weight = CW_Register;
			break;

		case 'f': // Floating-point register
			if (type->isFloatingPointTy()) weight = CW_Register;
			break;

		case 'I': // Unsigned 8-bit constant
			if (ConstantInt *C = dyn_cast<ConstantInt>(CallOperandVal)) if (isUInt<8>(C->getZExtValue())) weight = CW_Constant;
			break;

		case 'J': // Unsigned 12-bit constant
			if (ConstantInt *C = dyn_cast<ConstantInt>(CallOperandVal)) if (isUInt<12>(C->getZExtValue())) weight = CW_Constant;
			break;

		case 'K': // Signed 16-bit constant
			if (ConstantInt *C = dyn_cast<ConstantInt>(CallOperandVal)) if (isInt<16>(C->getSExtValue())) weight = CW_Constant;
			break;

		case 'L': // Signed 20-bit displacement (on all targets we support)
			if (ConstantInt *C = dyn_cast<ConstantInt>(CallOperandVal)) if (isInt<20>(C->getSExtValue())) weight = CW_Constant;
			break;

		case 'M': // 0x7fffffff
			if (ConstantInt *C = dyn_cast<ConstantInt>(CallOperandVal)) if (C->getZExtValue() == 0x7fffffff) weight = CW_Constant;
			break;
	}
	return weight;
}

std::pair<unsigned, const TargetRegisterClass *> REDEFINETargetLowering::getRegForInlineAsmConstraint(const std::string &Constraint, EVT VT) const {
	if (Constraint.size() == 1) {
		// GCC Constraint Letters
		switch (Constraint[0]) {
			default:
				break;
			case 'd': // Data register (equivalent to 'r')
			case 'r': // General-purpose register
				return std::make_pair(0U, &REDEFINE::GR32BitRegClass);

			case 'f': // Floating-point register
				return std::make_pair(0U, &REDEFINE::GR32BitRegClass);
		}
	}
	return TargetLowering::getRegForInlineAsmConstraint(Constraint, VT);
}

void REDEFINETargetLowering::LowerAsmOperandForConstraint(SDValue Op, std::string &Constraint, std::vector<SDValue> &Ops, SelectionDAG &DAG) const {
	// Only support length 1 constraints for now.
	if (Constraint.length() == 1) {
		switch (Constraint[0]) {
			case 'I': // Unsigned 8-bit constant
				if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) if (isUInt<8>(C->getZExtValue())) Ops.push_back(DAG.getTargetConstant(C->getZExtValue(), Op.getValueType()));
				return;

			case 'J': // Unsigned 12-bit constant
				if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) if (isUInt<12>(C->getZExtValue())) Ops.push_back(DAG.getTargetConstant(C->getZExtValue(), Op.getValueType()));
				return;

			case 'K': // Signed 16-bit constant
				if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) if (isInt<16>(C->getSExtValue())) Ops.push_back(DAG.getTargetConstant(C->getSExtValue(), Op.getValueType()));
				return;

			case 'L': // Signed 20-bit displacement (on all targets we support)
				if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) if (isInt<20>(C->getSExtValue())) Ops.push_back(DAG.getTargetConstant(C->getSExtValue(), Op.getValueType()));
				return;

			case 'M': // 0x7fffffff
				if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) if (C->getZExtValue() == 0x7fffffff) Ops.push_back(DAG.getTargetConstant(C->getZExtValue(), Op.getValueType()));
				return;
		}
	}
	TargetLowering::LowerAsmOperandForConstraint(Op, Constraint, Ops, DAG);
}

//===----------------------------------------------------------------------===//
//  Lower helper functions
//===----------------------------------------------------------------------===//

// addLiveIn - This helper function adds the specified physical register to the
// MachineFunction as a live in value.  It also creates a corresponding
// virtual register for it.
static unsigned addLiveIn(MachineFunction &MF, unsigned PReg, const TargetRegisterClass *RC) {
	unsigned VReg = MF.getRegInfo().createVirtualRegister(RC);
	MF.getRegInfo().addLiveIn(PReg, VReg);
	return VReg;
}

//===----------------------------------------------------------------------===//
// Calling conventions
//===----------------------------------------------------------------------===//

#include "REDEFINEGenCallingConv.inc"

// Value is a value that has been passed to us in the location described by VA
// (and so has type VA.getLocVT()).  Convert Value to VA.getValVT(), chaining
// any loads onto Chain.
static SDValue convertLocVTToValVT(SelectionDAG &DAG, DebugLoc DL, CCValAssign &VA, SDValue Chain, SDValue Value) {
	// If the argument has been promoted from a smaller type, insert an
	// assertion to capture this.
	if (VA.getLocInfo() == CCValAssign::SExt) Value = DAG.getNode(ISD::AssertSext, DL, VA.getLocVT(), Value, DAG.getValueType(VA.getValVT()));
	else if (VA.getLocInfo() == CCValAssign::ZExt) Value = DAG.getNode(ISD::AssertZext, DL, VA.getLocVT(), Value, DAG.getValueType(VA.getValVT()));

	if (VA.isExtInLoc()) Value = DAG.getNode(ISD::TRUNCATE, DL, VA.getValVT(), Value);
	else if (VA.getLocInfo() == CCValAssign::Indirect) Value = DAG.getLoad(VA.getValVT(), DL, Chain, Value, MachinePointerInfo(), false, false, false, 0);
	else
	assert(VA.getLocInfo() == CCValAssign::Full && "Unsupported getLocInfo");
	return Value;
}

// Value is a value of type VA.getValVT() that we need to copy into
// the location described by VA.  Return a copy of Value converted to
// VA.getValVT().  The caller is responsible for handling indirect values.
static SDValue convertValVTToLocVT(SelectionDAG &DAG, DebugLoc DL, CCValAssign &VA, SDValue Value) {
	switch (VA.getLocInfo()) {
		case CCValAssign::SExt:
			return DAG.getNode(ISD::SIGN_EXTEND, DL, VA.getLocVT(), Value);
		case CCValAssign::ZExt:
			return DAG.getNode(ISD::ZERO_EXTEND, DL, VA.getLocVT(), Value);
		case CCValAssign::AExt:
			return DAG.getNode(ISD::ANY_EXTEND, DL, VA.getLocVT(), Value);
		case CCValAssign::BCvt:
			return DAG.getNode(ISD::BITCAST, DL, VA.getLocVT(), Value);
		case CCValAssign::Full:
			return Value;
		default:
			llvm_unreachable("Unhandled getLocInfo()");
	}
}

/// This function returns true if CallSym is a long double emulation routine.
static bool isF128SoftLibCall(const char *CallSym) {
	const char * const LibCalls[] = { "__addtf3", "__divtf3", "__eqtf2", "__extenddftf2", "__extendsftf2", "__fixtfdi", "__fixtfsi", "__fixtfti", "__fixunstfdi", "__fixunstfsi", "__fixunstfti", "__floatditf", "__floatsitf", "__floattitf", "__floatunditf", "__floatunsitf", "__floatuntitf", "__getf2",
	        "__gttf2", "__letf2", "__lttf2", "__multf3", "__netf2", "__powitf2", "__subtf3", "__trunctfdf2", "__trunctfsf2", "__unordtf2", "ceill", "copysignl", "cosl", "exp2l", "expl", "floorl", "fmal", "fmodl", "log10l", "log2l", "logl", "nearbyintl", "powl", "rintl", "sinl", "sqrtl", "truncl" };

	const char * const *End = LibCalls + array_lengthof(LibCalls);

	// Check that LibCalls is sorted alphabetically.
	REDEFINETargetLowering::LTStr Comp;

#ifndef NDEBUG
	for (const char * const *I = LibCalls; I < End - 1; ++I)
		assert(Comp(*I, *(I + 1)));
#endif

	//return std::binary_search(LibCalls, End, CallSym, Comp);
	return false;
}

/// This function returns true if Ty is fp128 or i128 which was originally a
/// fp128.
static bool originalTypeIsF128(const Type *Ty, const SDNode *CallNode) {
	if (Ty->isFP128Ty()) return true;

	const ExternalSymbolSDNode *ES = dyn_cast_or_null<const ExternalSymbolSDNode>(CallNode);

	// If the Ty is i128 and the function being called is a long double emulation
	// routine, then the original type is f128.
	return (ES && Ty->isIntegerTy(128) && isF128SoftLibCall(ES->getSymbol()));
}

//REDEFINECC Implementation
REDEFINETargetLowering::REDEFINECC::REDEFINECC(CallingConv::ID CC, bool IsREDEFINE32_, CCState &Info, const REDEFINESubtarget &Subtarget) :
		CCInfo(Info), CallConv(CC), IsREDEFINE32(IsREDEFINE32_), Subtarget(Subtarget) {
	// Pre-allocate reserved argument area.
	CCInfo.AllocateStack(reservedArgArea(), 1);
}

void REDEFINETargetLowering::REDEFINECC::analyzeCallOperands(const SmallVectorImpl<ISD::OutputArg> &Args, bool IsVarArg, bool IsSoftFloat, const SDNode *CallNode, std::vector<ArgListEntry> &FuncArgs) {
	assert((CallConv != CallingConv::Fast || !IsVarArg) && "CallingConv::Fast shouldn't be used for vararg functions.");

	unsigned NumOpnds = Args.size();
	llvm::CCAssignFn *FixedFn = fixedArgFn(), *VarFn = varArgFn();

	for (unsigned I = 0; I != NumOpnds; ++I) {
		MVT ArgVT = Args[I].VT;
		ISD::ArgFlagsTy ArgFlags = Args[I].Flags;
		bool R;
		if (ArgFlags.isByVal()) {
			handleByValArg(I, ArgVT, ArgVT, CCValAssign::Full, ArgFlags);
			continue;
		}

		if (IsVarArg && !Args[I].IsFixed) if (ArgVT.isFloatingPoint()) //Bitconvert floats
		R = VarFn(I, ArgVT, ArgVT, CCValAssign::BCvt, ArgFlags, CCInfo);
		else R = VarFn(I, ArgVT, ArgVT, CCValAssign::Full, ArgFlags, CCInfo);
		else {
			MVT RegVT = getRegVT(ArgVT, FuncArgs[Args[I].OrigArgIndex].Ty, CallNode, IsSoftFloat);
			R = FixedFn(I, ArgVT, RegVT, CCValAssign::Full, ArgFlags, CCInfo);
		}

		if (R) {
#ifndef NDEBUG
			dbgs() << "Call operand #" << I << " has unhandled type " << EVT(ArgVT).getEVTString();
#endif
			llvm_unreachable(0);
		}
	}
}

void REDEFINETargetLowering::REDEFINECC::analyzeFormalArguments(const SmallVectorImpl<ISD::InputArg> &Args, bool IsSoftFloat, Function::const_arg_iterator FuncArg) {
	unsigned NumArgs = Args.size();
	llvm::CCAssignFn *FixedFn;
	FixedFn = CC_REDEFINE32;
	unsigned CurArgIdx = 0;
	for (unsigned I = 0; I != NumArgs; ++I) {
		MVT ArgVT = Args[I].VT;
		ISD::ArgFlagsTy ArgFlags = Args[I].Flags;
		std::advance(FuncArg, Args[I].OrigArgIndex - CurArgIdx);
		CurArgIdx = Args[I].OrigArgIndex;
		if (ArgFlags.isByVal()) {
			handleByValArg(I, ArgVT, ArgVT, CCValAssign::Full, ArgFlags);
			continue;
		}

		if (ArgFlags.isSRet()) {

		}

		MVT RegVT = getRegVT(ArgVT, FuncArg->getType(), 0, IsSoftFloat);

		if (!FixedFn(I, ArgVT, RegVT, CCValAssign::Full, ArgFlags, CCInfo)) continue;

#ifndef NDEBUG
		dbgs() << "Formal Arg #" << I << " has unhandled type " << EVT(ArgVT).getEVTString();
#endif
		llvm_unreachable(0);
	}
}

MVT REDEFINETargetLowering::REDEFINECC::getRegVT(MVT VT, const Type *OrigTy, const SDNode *CallNode, bool IsSoftFloat) const {
	if (IsSoftFloat || IsREDEFINE32) return VT;

	// Check if the original type was fp128.
	if (originalTypeIsF128(OrigTy, CallNode)) {
		assert(VT == MVT::i64);
		return MVT::f64;
	}

	return VT;
}

void REDEFINETargetLowering::copyByValRegs(SDValue Chain, DebugLoc DL, std::vector<SDValue> &OutChains, SelectionDAG &DAG, const ISD::ArgFlagsTy &Flags, SmallVectorImpl<SDValue> &InVals, const Argument *FuncArg, const REDEFINECC &CC, const ByValArgInfo &ByVal) const {
	MachineFunction &MF = DAG.getMachineFunction();
	MachineFrameInfo *MFI = MF.getFrameInfo();
	unsigned RegAreaSize = ByVal.NumRegs * CC.regSize();
	unsigned FrameObjSize = std::max(Flags.getByValSize(), RegAreaSize);
	int FrameObjOffset;

	if (RegAreaSize) FrameObjOffset = (int) CC.reservedArgArea() - (int) ((CC.numIntArgRegs() - ByVal.FirstIdx) * CC.regSize());
	else FrameObjOffset = ByVal.Address;

	// Create frame object.
	EVT PtrTy = getPointerTy();
	int FI = MFI->CreateFixedObject(FrameObjSize, FrameObjOffset, true);
	SDValue FIN = DAG.getFrameIndex(FI, PtrTy);
	InVals.push_back(FIN);

	if (!ByVal.NumRegs) return;

	// Copy arg registers.
	MVT RegTy = MVT::getIntegerVT(CC.regSize() * 8);
	const TargetRegisterClass *RC = getRegClassFor(RegTy);

	for (unsigned I = 0; I < ByVal.NumRegs; ++I) {
		unsigned ArgReg = CC.intArgRegs()[ByVal.FirstIdx + I];
		unsigned VReg = addLiveIn(MF, ArgReg, RC);
		unsigned Offset = I * CC.regSize();
		SDValue StorePtr = DAG.getNode(ISD::ADD, DL, PtrTy, FIN, DAG.getConstant(Offset, PtrTy));
		SDValue Store = DAG.getStore(Chain, DL, DAG.getRegister(VReg, RegTy), StorePtr, MachinePointerInfo(FuncArg, Offset), false, false, 0);
		OutChains.push_back(Store);
	}
}

// Copy byVal arg to registers and stack.
void REDEFINETargetLowering::passByValArg(SDValue Chain, DebugLoc DL, std::deque<std::pair<unsigned, SDValue> > &RegsToPass, SmallVector<SDValue, 8> &MemOpChains, SDValue StackPtr, MachineFrameInfo *MFI, SelectionDAG &DAG, SDValue Arg, const REDEFINECC &CC, const ByValArgInfo &ByVal,
        const ISD::ArgFlagsTy &Flags, bool isLittle) const {
	unsigned ByValSize = Flags.getByValSize();
	unsigned Offset = 0; // Offset in # of bytes from the beginning of struct.
	unsigned RegSize = CC.regSize();
	unsigned Alignment = std::min(Flags.getByValAlign(), RegSize);
	EVT PtrTy = getPointerTy(), RegTy = MVT::getIntegerVT(RegSize * 8);

	if (ByVal.NumRegs) {
		const uint16_t *ArgRegs = CC.intArgRegs();
		bool LeftoverBytes = (ByVal.NumRegs * RegSize > ByValSize);
		unsigned I = 0;

		// Copy words to registers.
		for (; I < ByVal.NumRegs - LeftoverBytes; ++I, Offset += RegSize) {
			SDValue LoadPtr = DAG.getNode(ISD::ADD, DL, PtrTy, Arg, DAG.getConstant(Offset, PtrTy));
			SDValue LoadVal = DAG.getLoad(RegTy, DL, Chain, LoadPtr, MachinePointerInfo(), false, false, false, Alignment);
			MemOpChains.push_back(LoadVal.getValue(1));
			unsigned ArgReg = ArgRegs[ByVal.FirstIdx + I];
			RegsToPass.push_back(std::make_pair(ArgReg, LoadVal));
		}

		// Return if the struct has been fully copied.
		if (ByValSize == Offset) return;

		// Copy the remainder of the byval argument with sub-word loads and shifts.
		if (LeftoverBytes) {
			assert((ByValSize > Offset) && (ByValSize < Offset + RegSize) && "Size of the remainder should be smaller than RegSize.");
			SDValue Val;

			for (unsigned LoadSize = RegSize / 2, TotalSizeLoaded = 0; Offset < ByValSize; LoadSize /= 2) {
				unsigned RemSize = ByValSize - Offset;

				if (RemSize < LoadSize) continue;

				// Load subword.
				SDValue LoadPtr = DAG.getNode(ISD::ADD, DL, PtrTy, Arg, DAG.getConstant(Offset, PtrTy));
				SDValue LoadVal = DAG.getExtLoad(ISD::ZEXTLOAD, DL, RegTy, Chain, LoadPtr, MachinePointerInfo(), MVT::getIntegerVT(LoadSize * 8), false, false, Alignment);
				MemOpChains.push_back(LoadVal.getValue(1));

				// Shift the loaded value.
				unsigned Shamt;

				if (isLittle) Shamt = TotalSizeLoaded;
				else Shamt = (RegSize - (TotalSizeLoaded + LoadSize)) * 8;

				SDValue Shift = DAG.getNode(ISD::SHL, DL, RegTy, LoadVal, DAG.getConstant(Shamt, MVT::i32));

				if (Val.getNode()) Val = DAG.getNode(ISD::OR, DL, RegTy, Val, Shift);
				else Val = Shift;

				Offset += LoadSize;
				TotalSizeLoaded += LoadSize;
				Alignment = std::min(Alignment, LoadSize);
			}

			unsigned ArgReg = ArgRegs[ByVal.FirstIdx + I];
			RegsToPass.push_back(std::make_pair(ArgReg, Val));
			return;
		}
	}

	// Copy remainder of byval arg to it with memcpy.
	unsigned MemCpySize = ByValSize - Offset;
	SDValue Src = DAG.getNode(ISD::ADD, DL, PtrTy, Arg, DAG.getConstant(Offset, PtrTy));
	SDValue Dst = DAG.getNode(ISD::ADD, DL, PtrTy, StackPtr, DAG.getIntPtrConstant(ByVal.Address));
	Chain = DAG.getMemcpy(Chain, DL, Dst, Src, DAG.getConstant(MemCpySize, PtrTy), Alignment,
	/*isVolatile=*/false, /*AlwaysInline=*/false, MachinePointerInfo(0), MachinePointerInfo(0));
	MemOpChains.push_back(Chain);
}

void REDEFINETargetLowering::REDEFINECC::handleByValArg(unsigned ValNo, MVT ValVT, MVT LocVT, CCValAssign::LocInfo LocInfo, ISD::ArgFlagsTy ArgFlags) {
	assert(ArgFlags.getByValSize() && "Byval argument's size shouldn't be 0.");

	struct ByValArgInfo ByVal;
	unsigned RegSize = regSize();
	unsigned ByValSize = RoundUpToAlignment(ArgFlags.getByValSize(), RegSize);
	unsigned Align = std::min(std::max(ArgFlags.getByValAlign(), RegSize), RegSize * 2);

	if (useRegsForByval()) allocateRegs(ByVal, ByValSize, Align, ValVT);

	// Allocate space on caller's stack.
	ByVal.Address = CCInfo.AllocateStack(ByValSize - RegSize * ByVal.NumRegs, Align);
	CCInfo.addLoc(CCValAssign::getMem(ValNo, ValVT, ByVal.Address, LocVT, LocInfo));
	ByValArgs.push_back(ByVal);
}

unsigned REDEFINETargetLowering::REDEFINECC::numIntArgRegs() const {
	return llvm::REDEFINE::NumArgGPRs;
}

unsigned REDEFINETargetLowering::REDEFINECC::numFPArgRegs() const {
	return llvm::REDEFINE::NumArgFPRs;
}

unsigned REDEFINETargetLowering::REDEFINECC::reservedArgArea() const {
	return 0;
}

const uint16_t *REDEFINETargetLowering::REDEFINECC::intArgRegs() const {
	return REDEFINE32IntRegs;
}

const uint16_t *REDEFINETargetLowering::REDEFINECC::fpArgRegs() const {
	return intArgRegs();
}

llvm::CCAssignFn *REDEFINETargetLowering::REDEFINECC::fixedArgFn() const {
	return CC_REDEFINE32;
}

llvm::CCAssignFn *REDEFINETargetLowering::REDEFINECC::varArgFn() const {
	return CC_REDEFINE32_VAR;
}

void REDEFINETargetLowering::REDEFINECC::allocateRegs(ByValArgInfo &ByVal, unsigned ByValSize, unsigned Align, MVT ValVT) {
	errs()<<"allocating register to:"<<ByVal.Address<<"\n";
	unsigned RegSize = regSize();
	if (ValVT.isInteger()) {
		unsigned NumIntArgRegs = numIntArgRegs();
		const uint16_t *IntArgRegs = intArgRegs(); //, *ShadowRegs = shadowRegs();
		assert(!(ByValSize % RegSize) && !(Align % RegSize) && "Byval argument's size and alignment should be a multiple of"
				"RegSize.");

		ByVal.FirstIdx = CCInfo.getFirstUnallocated(IntArgRegs, NumIntArgRegs);

		// If Align > RegSize, the first arg register must be even.
		if ((Align > RegSize) && (ByVal.FirstIdx % 2)) {
			CCInfo.AllocateReg(IntArgRegs[ByVal.FirstIdx]); //, ShadowRegs[ByVal.FirstIdx]);
			++ByVal.FirstIdx;
		}

		// Mark the registers allocated.
		for (unsigned I = ByVal.FirstIdx; ByValSize && (I < NumIntArgRegs); ByValSize -= RegSize, ++I, ++ByVal.NumRegs)
			CCInfo.AllocateReg(IntArgRegs[I]); //, ShadowRegs[I]);
	}
	else if (ValVT.isFloatingPoint()) {
		RegSize = fpRegSize();
		unsigned NumFPArgRegs = numFPArgRegs();
		const uint16_t *FPArgRegs = fpArgRegs(); //, *ShadowRegs = shadowRegs();
		assert(!(ByValSize % RegSize) && !(Align % RegSize) && "Byval argument's size and alignment should be a multiple of"
				"RegSize.");

		ByVal.FirstIdx = CCInfo.getFirstUnallocated(FPArgRegs, NumFPArgRegs);

		// If Align > RegSize, the first arg register must be even.
		if ((Align > RegSize) && (ByVal.FirstIdx % 2)) {
			CCInfo.AllocateReg(FPArgRegs[ByVal.FirstIdx]); //, ShadowRegs[ByVal.FirstIdx]);
			++ByVal.FirstIdx;
		}

		// Mark the registers allocated.
		for (unsigned I = ByVal.FirstIdx; ByValSize && (I < NumFPArgRegs); ByValSize -= RegSize, ++I, ++ByVal.NumRegs)
			CCInfo.AllocateReg(FPArgRegs[I]); //, ShadowRegs[I]);
	}
	else
	llvm_unreachable("Cannot pass this type by value");
}
//End REDEFINECC Implementation

SDValue REDEFINETargetLowering::LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg, const SmallVectorImpl<ISD::InputArg> &Ins, DebugLoc DL, SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
	MachineFunction &MF = DAG.getMachineFunction();
	MachineFrameInfo *MFI = MF.getFrameInfo();

//	REDEFINEFI->setVarArgsFrameIndex(0);

	// Used with vargs to acumulate store chains.
	std::vector<SDValue> OutChains;

	// Assign locations to all of the incoming arguments.
	SmallVector<CCValAssign, 16> ArgLocs;
	CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), getTargetMachine(), ArgLocs, *DAG.getContext());

	REDEFINECC REDEFINECCInfo(CallConv, IsREDEFINE32, CCInfo, Subtarget);

	Function::const_arg_iterator FuncArg = DAG.getMachineFunction().getFunction()->arg_begin();
	bool UseSoftFloat = getTargetMachine().Options.UseSoftFloat;

	REDEFINECCInfo.analyzeFormalArguments(Ins, UseSoftFloat, FuncArg);

	unsigned CurArgIdx = 0;
	REDEFINECC::byval_iterator ByValArg = REDEFINECCInfo.byval_begin();

	for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
		CCValAssign &VA = ArgLocs[i];
		std::advance(FuncArg, Ins[i].OrigArgIndex - CurArgIdx);
		CurArgIdx = Ins[i].OrigArgIndex;
		EVT ValVT = VA.getValVT();
		ISD::ArgFlagsTy Flags = Ins[i].Flags;
		bool IsRegLoc = VA.isRegLoc();

		if (Flags.isByVal()) {
			assert(Flags.getByValSize() && "ByVal args of size 0 should have been ignored by front-end.");
			assert(ByValArg != REDEFINECCInfo.byval_end());
			copyByValRegs(Chain, DL, OutChains, DAG, Flags, InVals, &*FuncArg, REDEFINECCInfo, *ByValArg);
			++ByValArg;
			continue;
		}

		// Arguments stored on registers
		if (IsRegLoc) {
			EVT RegVT = VA.getLocVT();
			unsigned ArgReg = VA.getLocReg();
			const TargetRegisterClass *RC;

			RC = &REDEFINE::GR32BitRegClass;

			// Transform the arguments stored on
			// physical registers into virtual ones
			unsigned Reg = addLiveIn(DAG.getMachineFunction(), ArgReg, RC);
			SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, Reg, RegVT);

			// If this is an 8 or 16-bit value, it has been passed promoted
			// to 32 bits.  Insert an assert[sz]ext to capture this, then
			// truncate to the right size.
			if (VA.getLocInfo() != CCValAssign::Full) {
				unsigned Opcode = 0;
				if (VA.getLocInfo() == CCValAssign::SExt) Opcode = ISD::AssertSext;
				else if (VA.getLocInfo() == CCValAssign::ZExt) Opcode = ISD::AssertZext;
				if (Opcode) ArgValue = DAG.getNode(Opcode, DL, RegVT, ArgValue, DAG.getValueType(ValVT));
				ArgValue = DAG.getNode(ISD::TRUNCATE, DL, ValVT, ArgValue);
			}

			InVals.push_back(ArgValue);
		}
		else { // VA.isRegLoc()

			// sanity check
			assert(VA.isMemLoc());

			// The stack pointer offset is relative to the caller stack frame.
			int FI = MFI->CreateFixedObject(ValVT.getSizeInBits() / 8, VA.getLocMemOffset(), true);

			// Create load nodes to retrieve arguments from the stack
			SDValue FIN = DAG.getFrameIndex(FI, getPointerTy());
			InVals.push_back(DAG.getLoad(ValVT, DL, Chain, FIN, MachinePointerInfo::getFixedStack(FI), false, false, false, 0));
		}
	}

//	if (IsVarArg)
//		writeVarArgRegs(OutChains, REDEFINECCInfo, Chain, DL, DAG);

	// All stores are grouped in one node to allow the matching between
	// the size of Ins and InVals. This only happens when on varg functions
	if (!OutChains.empty()) {
		OutChains.push_back(Chain);
		Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, &OutChains[0], OutChains.size());
	}

	return Chain;
}

static SDValue getTargetNode(SDValue Op, SelectionDAG &DAG, unsigned Flag) {
	EVT Ty = Op.getValueType();

	if (GlobalAddressSDNode *N = dyn_cast<GlobalAddressSDNode>(Op)) return DAG.getTargetGlobalAddress(N->getGlobal(), Op.getDebugLoc(), Ty, 0, Flag);
	if (ExternalSymbolSDNode *N = dyn_cast<ExternalSymbolSDNode>(Op)) return DAG.getTargetExternalSymbol(N->getSymbol(), Ty, Flag);
	if (BlockAddressSDNode *N = dyn_cast<BlockAddressSDNode>(Op)) return DAG.getTargetBlockAddress(N->getBlockAddress(), Ty, 0, Flag);
	if (JumpTableSDNode *N = dyn_cast<JumpTableSDNode>(Op)) return DAG.getTargetJumpTable(N->getIndex(), Ty, Flag);
	if (ConstantPoolSDNode *N = dyn_cast<ConstantPoolSDNode>(Op)) return DAG.getTargetConstantPool(N->getConstVal(), Ty, N->getAlignment(), N->getOffset(), Flag);

	llvm_unreachable("Unexpected node type.");
	return SDValue();
}

static SDValue getAddrNonPIC(SDValue Op, SelectionDAG &DAG) {
	DebugLoc DL = Op.getDebugLoc();
	EVT Ty = Op.getValueType();
	SDValue Hi = getTargetNode(Op, DAG, REDEFINEII::MO_ABS_HI);
	SDValue Lo = getTargetNode(Op, DAG, REDEFINEII::MO_ABS_LO);
	SDValue ResHi = DAG.getNode(REDEFINEISD::Hi, DL, Ty, Hi);
	SDValue ResLo = DAG.getNode(REDEFINEISD::Lo, DL, Ty, Lo);
	return DAG.getNode(ISD::ADD, DL, Ty, ResHi, ResLo);
}

static SDValue getAddrPIC(SDValue Op, SelectionDAG &DAG) {
	DebugLoc DL = Op.getDebugLoc();
	EVT Ty = Op.getValueType();
	return DAG.getNode(REDEFINEISD::PCREL_WRAPPER, DL, Ty, Op);
}

SDValue REDEFINETargetLowering::LowerCall(CallLoweringInfo &CLI, SmallVectorImpl<SDValue> &InVals) const {
	SelectionDAG &DAG = CLI.DAG;
	DebugLoc &DL = CLI.DL;
	SmallVector<ISD::OutputArg, 32> &Outs = CLI.Outs;
	SmallVector<SDValue, 32> &OutVals = CLI.OutVals;
	SmallVector<ISD::InputArg, 32> &Ins = CLI.Ins;
	SDValue Chain = CLI.Chain;
	SDValue Callee = CLI.Callee;
	bool &isTailCall = CLI.IsTailCall;
	CallingConv::ID CallConv = CLI.CallConv;
	bool IsVarArg = CLI.IsVarArg;
	MachineFunction &MF = DAG.getMachineFunction();
	MachineFrameInfo *MFI = MF.getFrameInfo();
	EVT PtrVT = getPointerTy();

	// REDEFINE target does not yet support tail call optimization.
	isTailCall = false;

	// Analyze the operands of the call, assigning locations to each operand.
	SmallVector<CCValAssign, 16> ArgLocs;
	CCState ArgCCInfo(CallConv, IsVarArg, MF, TM, ArgLocs, *DAG.getContext());
	REDEFINECC REDEFINECCInfo(CallConv, Subtarget.isREDEFINE32(), ArgCCInfo, Subtarget);
	REDEFINECCInfo.analyzeCallOperands(Outs, IsVarArg, getTargetMachine().Options.UseSoftFloat, Callee.getNode(), CLI.Args);

	// Get a count of how many bytes are to be pushed on the stack.
	unsigned NumBytes = ArgCCInfo.getNextStackOffset();

	// Mark the start of the call.
	Chain = DAG.getCALLSEQ_START(Chain, DAG.getConstant(NumBytes, PtrVT, true));

	REDEFINECC::byval_iterator ByValArg = REDEFINECCInfo.byval_begin();

	// Copy argument values to their designated locations.
	std::deque<std::pair<unsigned, SDValue> > RegsToPass;
	SmallVector<SDValue, 8> MemOpChains;
	SDValue StackPtr;
	for (unsigned I = 0, E = ArgLocs.size(); I != E; ++I) {
		CCValAssign &VA = ArgLocs[I];
		SDValue ArgValue = OutVals[I];
		ISD::ArgFlagsTy Flags = Outs[I].Flags;

		// ByVal Arg.
		if (Flags.isByVal()) {
			assert(Flags.getByValSize() && "ByVal args of size 0 should have been ignored by front-end.");
			assert(ByValArg != REDEFINECCInfo.byval_end());
			assert(!isTailCall && "Do not tail-call optimize if there is a byval argument.");
			if (!StackPtr.getNode()) StackPtr = DAG.getCopyFromReg(Chain, DL, REDEFINE::sp, PtrVT);
			passByValArg(Chain, DL, RegsToPass, MemOpChains, StackPtr, MFI, DAG, ArgValue, REDEFINECCInfo, *ByValArg, Flags, true);  // Subtarget is little endian so we have true
			++ByValArg;
			continue;
		}

		ArgValue = convertValVTToLocVT(DAG, DL, VA, ArgValue);

		if (VA.isRegLoc())
		// Queue up the argument copies and emit them at the end.
		RegsToPass.push_back(std::make_pair(VA.getLocReg(), ArgValue));
		else {
			assert(VA.isMemLoc() && "Argument not register or memory");

			// Work out the address of the stack slot.  Unpromoted ints and
			// floats are passed as right-justified 8-byte values.
			if (!StackPtr.getNode()) StackPtr = DAG.getCopyFromReg(Chain, DL, REDEFINE::sp, PtrVT);
			unsigned Offset = VA.getLocMemOffset();
			SDValue Address = DAG.getNode(ISD::ADD, DL, PtrVT, StackPtr, DAG.getIntPtrConstant(Offset));

			// Emit the store.
			MemOpChains.push_back(DAG.getStore(Chain, DL, ArgValue, Address, MachinePointerInfo(), false, false, 0));
		}
	}

	// Join the stores, which are independent of one another.
	if (!MemOpChains.empty()) Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, &MemOpChains[0], MemOpChains.size());

	// Build a sequence of copy-to-reg nodes, chained and glued together.
	SDValue Glue;
	for (unsigned I = 0, E = RegsToPass.size(); I != E; ++I) {
		Chain = DAG.getCopyToReg(Chain, DL, RegsToPass[I].first, RegsToPass[I].second, Glue);
		Glue = Chain.getValue(1);
	}

	// Accept direct calls by converting symbolic call addresses to the
	// associated Target* opcodes.
	if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee)) {
		if (TM.getRelocationModel() == Reloc::PIC_) {
			Callee = getAddrPIC(Callee, DAG);
		}
		else Callee = DAG.getTargetGlobalAddress(G->getGlobal(), DL, PtrVT);
	}
	else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee)) {
		if (TM.getRelocationModel() == Reloc::PIC_) {
			Callee = getAddrPIC(DAG.getTargetExternalSymbol(E->getSymbol(), PtrVT), DAG);
		}
		else Callee = DAG.getTargetExternalSymbol(E->getSymbol(), PtrVT);
	}

	// The first call operand is the chain and the second is the target address.
	SmallVector<SDValue, 8> Ops;
	Ops.push_back(Chain);
	Ops.push_back(Callee);

	// Add argument registers to the end of the list so that they are
	// known live into the call.
	for (unsigned I = 0, E = RegsToPass.size(); I != E; ++I)
		Ops.push_back(DAG.getRegister(RegsToPass[I].first, RegsToPass[I].second.getValueType()));

	// Glue the call to the argument copies, if any.
	if (Glue.getNode()) Ops.push_back(Glue);

	// Emit the call.
	SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
	Chain = DAG.getNode(REDEFINEISD::CALL, DL, NodeTys, &Ops[0], Ops.size());
	Glue = Chain.getValue(1);

	// Mark the end of the call, which is glued to the call itself.
	Chain = DAG.getCALLSEQ_END(Chain, DAG.getConstant(NumBytes, PtrVT, true), DAG.getConstant(0, PtrVT, true), Glue);
	Glue = Chain.getValue(1);

	// Assign locations to each value returned by this call.
	SmallVector<CCValAssign, 16> RetLocs;
	CCState RetCCInfo(CallConv, IsVarArg, MF, TM, RetLocs, *DAG.getContext());
	RetCCInfo.AnalyzeCallResult(Ins, RetCC_REDEFINE32);

	// Copy all of the result registers out of their specified physreg.
	for (unsigned I = 0, E = RetLocs.size(); I != E; ++I) {
		CCValAssign &VA = RetLocs[I];

		// Copy the value out, gluing the copy to the end of the call sequence.
		SDValue RetValue = DAG.getCopyFromReg(Chain, DL, VA.getLocReg(), VA.getLocVT(), Glue);
		Chain = RetValue.getValue(1);
		Glue = RetValue.getValue(2);

		// Convert the value of the return register into the value that's
		// being returned.
		InVals.push_back(convertLocVTToValVT(DAG, DL, VA, Chain, RetValue));
	}

	return Chain;
}

/// This hook should be implemented to check whether the return values
/// described by the Outs array can fit into the return registers.  If false
/// is returned, an sret-demotion is performed.
bool REDEFINETargetLowering::CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF, bool IsVarArg, const SmallVectorImpl<ISD::OutputArg> &Outs, LLVMContext &Context) const {
	SmallVector<CCValAssign, 16> RVLocs;
	CCState CCInfo(CallConv, IsVarArg, MF, getTargetMachine(), RVLocs, Context);
	return CCInfo.CheckReturn(Outs, RetCC_REDEFINE32);
}

SDValue REDEFINETargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg, const SmallVectorImpl<ISD::OutputArg> &Outs, const SmallVectorImpl<SDValue> &OutVals, DebugLoc DL, SelectionDAG &DAG) const {
	MachineFunction &MF = DAG.getMachineFunction();

	// Assign locations to each returned value.
	SmallVector<CCValAssign, 16> RetLocs;
	CCState RetCCInfo(CallConv, IsVarArg, MF, TM, RetLocs, *DAG.getContext());
	RetCCInfo.AnalyzeReturn(Outs, RetCC_REDEFINE32);

	SDValue Glue;
	// Quick exit for void returns
	if (RetLocs.empty()) return DAG.getNode(REDEFINEISD::RET_FLAG, DL, MVT::Other, Chain);

	// Copy the result values into the output registers.
	SmallVector<SDValue, 4> RetOps;
	RetOps.push_back(Chain);
	for (unsigned I = 0, E = RetLocs.size(); I != E; ++I) {
		CCValAssign &VA = RetLocs[I];
		SDValue RetValue = OutVals[I];

		// Make the return register live on exit.
		assert(VA.isRegLoc() && "Can only return in registers!");

		// Promote the value as required.
		RetValue = convertValVTToLocVT(DAG, DL, VA, RetValue);

		// Chain and glue the copies together.
		unsigned Reg = VA.getLocReg();
		Chain = DAG.getCopyToReg(Chain, DL, Reg, RetValue, Glue);
		Glue = Chain.getValue(1);
		RetOps.push_back(DAG.getRegister(Reg, VA.getLocVT()));
	}

	// Update chain and glue.
	RetOps[0] = Chain;
	if (Glue.getNode()) RetOps.push_back(Glue);

	return DAG.getNode(REDEFINEISD::RET_FLAG, DL, MVT::Other, RetOps.data(), RetOps.size());
}

SDValue REDEFINETargetLowering::lowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const {
	DebugLoc DL = Op.getDebugLoc();
	EVT Ty = Op.getOperand(0).getValueType();
	SDValue Cond = DAG.getNode(ISD::SETCC, DL, getSetCCResultType(Ty), Op.getOperand(0), Op.getOperand(1), Op.getOperand(4));

	return DAG.getNode(ISD::SELECT, DL, Op.getValueType(), Cond, Op.getOperand(2), Op.getOperand(3));
}

SDValue REDEFINETargetLowering::lowerRETURNADDR(SDValue Op, SelectionDAG &DAG) const {
	// check the depth
	//TODO: REDEFINE-gcc can handle this, by navigating through the stack, we should be able to do this too
	assert((cast<ConstantSDNode>(Op.getOperand(0))->getZExtValue() == 0) && "Return address can be determined only for current frame.");

	MachineFunction &MF = DAG.getMachineFunction();
	MachineFrameInfo *MFI = MF.getFrameInfo();
	MVT VT = Op.getSimpleValueType();
	unsigned RA = REDEFINE::ra;
	MFI->setReturnAddressIsTaken(true);

	// Return RA, which contains the return address. Mark it an implicit live-in.
	unsigned Reg = MF.addLiveIn(RA, getRegClassFor(VT));
	return DAG.getCopyFromReg(DAG.getEntryNode(), Op.getDebugLoc(), Reg, VT);
}

SDValue REDEFINETargetLowering::lowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const {
	Reloc::Model RM = TM.getRelocationModel();

	if (RM != Reloc::PIC_) {
		//%hi/%lo relocation
		return getAddrNonPIC(Op, DAG);
	}
	if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Op)) {
		Op = DAG.getTargetGlobalAddress(G->getGlobal(), Op->getDebugLoc(), getPointerTy());
		return Op;
	}
	llvm_unreachable("invalid global addresses to lower");
}

SDValue REDEFINETargetLowering::lowerGlobalTLSAddress(GlobalAddressSDNode *GA, SelectionDAG &DAG) const {
	// If the relocation model is PIC, use the General Dynamic TLS Model or
	// Local Dynamic TLS model, otherwise use the Initial Exec or
	// Local Exec TLS Model.

	DebugLoc DL = GA->getDebugLoc();
	const GlobalValue *GV = GA->getGlobal();
	EVT PtrVT = getPointerTy();

	TLSModel::Model model = getTargetMachine().getTLSModel(GV);

	SDValue Offset;
	if (model == TLSModel::LocalExec) {
		// Local Exec TLS Model
		assert(model == TLSModel::LocalExec);
		SDValue TGAHi = DAG.getTargetGlobalAddress(GV, DL, PtrVT, 0, REDEFINEII::MO_TPREL_HI);
		SDValue TGALo = DAG.getTargetGlobalAddress(GV, DL, PtrVT, 0, REDEFINEII::MO_TPREL_LO);
		SDValue Hi = DAG.getNode(REDEFINEISD::Hi, DL, PtrVT, TGAHi);
		SDValue Lo = DAG.getNode(REDEFINEISD::Lo, DL, PtrVT, TGALo);
		Offset = DAG.getNode(ISD::ADD, DL, PtrVT, Hi, Lo);
	}
	else {
		llvm_unreachable("only local-exec TLS mode supported");
	}

	//SDValue ThreadPointer = DAG.getNode(MipsISD::ThreadPointer, DL, PtrVT);
	SDValue ThreadPointer = DAG.getRegister(REDEFINE::tp, PtrVT);

	return DAG.getNode(ISD::ADD, DL, PtrVT, ThreadPointer, Offset);

}

SDValue REDEFINETargetLowering::lowerBlockAddress(BlockAddressSDNode *Node, SelectionDAG &DAG) const {
	const BlockAddress *BA = Node->getBlockAddress();
	int64_t Offset = Node->getOffset();
	EVT PtrVT = getPointerTy();

	SDValue Result = DAG.getTargetBlockAddress(BA, PtrVT, Offset);
	return Result;
}

SDValue REDEFINETargetLowering::lowerJumpTable(JumpTableSDNode *JT, SelectionDAG &DAG) const {
	DebugLoc DL = JT->getDebugLoc();
	EVT PtrVT = getPointerTy();
	SDValue Result = DAG.getTargetJumpTable(JT->getIndex(), PtrVT);

	// Use LARL to load the address of the table.
	return DAG.getNode(REDEFINEISD::PCREL_WRAPPER, DL, PtrVT, Result);
}

SDValue REDEFINETargetLowering::lowerConstantPool(ConstantPoolSDNode *CP, SelectionDAG &DAG) const {
	EVT PtrVT = getPointerTy();

	SDValue Result;
	if (CP->isMachineConstantPoolEntry()) Result = DAG.getTargetConstantPool(CP->getMachineCPVal(), PtrVT, CP->getAlignment());
	else Result = DAG.getTargetConstantPool(CP->getConstVal(), PtrVT, CP->getAlignment(), CP->getOffset());

	Reloc::Model RM = TM.getRelocationModel();

	if (RM != Reloc::PIC_) return getAddrNonPIC(Result, DAG);
	return getAddrPIC(Result, DAG);
}

SDValue REDEFINETargetLowering::lowerFRAMEADDR(SDValue Op, SelectionDAG &DAG) const {
	// check the depth
	assert((cast<ConstantSDNode>(Op.getOperand(0))->getZExtValue() == 0) && "Frame address can only be determined for current frame.");

	MachineFrameInfo *MFI = DAG.getMachineFunction().getFrameInfo();
	MFI->setFrameAddressIsTaken(true);
	EVT VT = Op.getValueType();
	DebugLoc DL = Op.getDebugLoc();
	SDValue FrameAddr = DAG.getCopyFromReg(DAG.getEntryNode(), DL, REDEFINE::fp, VT);
	return FrameAddr;
}

SDValue REDEFINETargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
	switch (Op.getOpcode()) {
		case ISD::RETURNADDR:
			return lowerRETURNADDR(Op, DAG);
		case ISD::SELECT_CC:
			return lowerSELECT_CC(Op, DAG);
		case ISD::GlobalAddress:
			return lowerGlobalAddress(Op, DAG);
		case ISD::GlobalTLSAddress:
			return lowerGlobalTLSAddress(cast<GlobalAddressSDNode>(Op), DAG);
		case ISD::BlockAddress:
			return lowerBlockAddress(cast<BlockAddressSDNode>(Op), DAG);
		case ISD::JumpTable:
			return lowerJumpTable(cast<JumpTableSDNode>(Op), DAG);
		case ISD::ConstantPool:
			return lowerConstantPool(cast<ConstantPoolSDNode>(Op), DAG);
		case ISD::FRAMEADDR:
			return lowerFRAMEADDR(Op, DAG);
		default:
			llvm_unreachable("Unexpected node to lower");
	}
}

const char *REDEFINETargetLowering::getTargetNodeName(unsigned Opcode) const {
#define OPCODE(NAME) case REDEFINEISD::NAME: return "REDEFINEISD::" #NAME
	switch (Opcode) {
		OPCODE(RET_FLAG)
;			OPCODE(CALL);
			OPCODE(PCREL_WRAPPER);
			OPCODE(Hi);
			OPCODE(Lo);
			OPCODE(SELECT_CC);
		}
		return NULL;
#undef OPCODE
	}

//===----------------------------------------------------------------------===//
// Custom insertion
//===----------------------------------------------------------------------===//

MachineBasicBlock *REDEFINETargetLowering::emitSelectCC(MachineInstr *MI, MachineBasicBlock *BB) const {

	const TargetInstrInfo *TII = getTargetMachine().getInstrInfo();
	DebugLoc DL = MI->getDebugLoc();

	// To "insert" a SELECT_CC instruction, we actually have to insert the
	// diamond control-flow pattern.  The incoming instruction knows the
	// destination vreg to set, the condition code register to branch on, the
	// true/false values to select between, and a branch opcode to use.
	const BasicBlock *LLVM_BB = BB->getBasicBlock();
	MachineFunction::iterator It = BB;
	++It;

	//  thisMBB:
	//  ...
	//   TrueVal = ...
	//   setcc r1, r2, r3
	//   bNE   r1, r0, copy1MBB
	//   fallthrough --> copy0MBB
	MachineBasicBlock *thisMBB = BB;
	MachineFunction *F = BB->getParent();
	MachineBasicBlock *copy0MBB = F->CreateMachineBasicBlock(LLVM_BB);
	MachineBasicBlock *sinkMBB = F->CreateMachineBasicBlock(LLVM_BB);
	F->insert(It, copy0MBB);
	F->insert(It, sinkMBB);

	// Transfer the remainder of BB and its successor edges to sinkMBB.
	sinkMBB->splice(sinkMBB->begin(), BB, llvm::next(MachineBasicBlock::iterator(MI)), BB->end());
	sinkMBB->transferSuccessorsAndUpdatePHIs(BB);

	// Next, add the true and fallthrough blocks as its successors.
	BB->addSuccessor(copy0MBB);
	BB->addSuccessor(sinkMBB);

	const TargetRegisterInfo *TRI = getTargetMachine().getRegisterInfo();
	const TargetRegisterClass *RC = MI->getRegClassConstraint(1, TII, TRI);
	unsigned bne = REDEFINE::BNE;
	unsigned zero = REDEFINE::zero;
	BuildMI(BB, DL, TII->get(bne)).addMBB(sinkMBB).addReg(zero).addReg(MI->getOperand(1).getReg());

	//  copy0MBB:
	//   %FalseValue = ...
	//   # fallthrough to sinkMBB
	BB = copy0MBB;

	// Update machine-CFG edges
	BB->addSuccessor(sinkMBB);

	//  sinkMBB:
	//   %Result = phi [ %TrueValue, thisMBB ], [ %FalseValue, copy0MBB ]
	//  ...
	BB = sinkMBB;

	//assume there is only one use of zero
	if (MI->getOperand(0).getReg() == REDEFINE::zero) {
		//Create a virtual register for zero and make a copy into it
		const TargetRegisterClass *RC = &REDEFINE::GR32BitRegClass;
		unsigned VReg = F->getRegInfo().createVirtualRegister(RC);
		BuildMI(*copy0MBB, copy0MBB->begin(), DL, TII->get(TargetOpcode::COPY), VReg).addReg(MI->getOperand(0).getReg());
		//Do the actual phi using the virtual reg now
		BuildMI(*BB, BB->begin(), DL, TII->get(REDEFINE::PHI), VReg).addReg(MI->getOperand(3).getReg()).addMBB(copy0MBB).addReg(MI->getOperand(2).getReg()).addMBB(thisMBB);
	}
	else if (MI->getOperand(2).getReg() == REDEFINE::zero) {
		//Create a virtual register for zero and make a copy into it
		const TargetRegisterClass *RC = &REDEFINE::GR32BitRegClass;
		unsigned VReg = F->getRegInfo().createVirtualRegister(RC);
		BuildMI(*copy0MBB, copy0MBB->begin(), DL, TII->get(TargetOpcode::COPY), VReg).addReg(MI->getOperand(2).getReg());
		//Do the actual phi using the virtual reg now
		BuildMI(*BB, BB->begin(), DL, TII->get(REDEFINE::PHI), MI->getOperand(0).getReg()).addReg(MI->getOperand(3).getReg()).addMBB(thisMBB).addReg(VReg).addMBB(copy0MBB);
	}
	else if (MI->getOperand(3).getReg() == REDEFINE::zero) {
		//Create a virtual register for zero and make a copy into it
		const TargetRegisterClass *RC = &REDEFINE::GR32BitRegClass;
		unsigned VReg = F->getRegInfo().createVirtualRegister(RC);
		BuildMI(*copy0MBB, copy0MBB->begin(), DL, TII->get(TargetOpcode::COPY), VReg).addReg(MI->getOperand(3).getReg());
		//Do the actual phi using the virtual reg now
		BuildMI(*BB, BB->begin(), DL, TII->get(REDEFINE::PHI), MI->getOperand(0).getReg()).addReg(VReg).addMBB(copy0MBB).addReg(MI->getOperand(2).getReg()).addMBB(thisMBB);
	}
	else {
		//None of the registers is zero so everything is already a virt reg
		BuildMI(*BB, BB->begin(), DL, TII->get(REDEFINE::PHI), MI->getOperand(0).getReg()).addReg(MI->getOperand(3).getReg()).addMBB(copy0MBB).addReg(MI->getOperand(2).getReg()).addMBB(thisMBB);
	}

	MI->eraseFromParent();   // The pseudo instruction is gone now.
	return BB;
}

MachineBasicBlock *REDEFINETargetLowering::EmitInstrWithCustomInserter(MachineInstr *MI, MachineBasicBlock *MBB) const {
	switch (MI->getOpcode()) {
		case REDEFINE::SELECT_CC:
			return emitSelectCC(MI, MBB);
		default:
			llvm_unreachable("Unexpected instr type to insert");
	}
}


//=- REDEFINECallingConv.td - Calling conventions for REDEFINE -*- tablegen -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for the REDEFINE ABI.
//===----------------------------------------------------------------------===//

class CCIfExtend<CCAction A>
  : CCIf<"ArgFlags.isSExt() || ArgFlags.isZExt()", A>;

/// CCIfSubtarget - Match if the current subtarget has a feature F.
class CCIfSubtarget<string F, CCAction A>:
  CCIf<!strconcat("State.getTarget().getSubtarget<REDEFINESubtarget>().", F), A>;

//===----------------------------------------------------------------------===//
// RV32 return value calling convention
//===----------------------------------------------------------------------===//
def RetCC_REDEFINE32 : CallingConv<[

  //First two return values go in a0,a1
  CCIfType<[i32], CCAssignToReg<[a0, a1]>>,

  CCIfType<[f32], CCAssignToReg<[a0, a1]>>
  //Falling off the end of allocation here leads to SRet demotion
]>;

//===----------------------------------------------------------------------===//
// REDEFINE32 argument calling conventions
//===----------------------------------------------------------------------===//
def CC_REDEFINE32 : CallingConv<[
  CCIfInReg<CCIfType<[i32, i64], CCAssignToReg<
                   [s1, a0, a1, a2, a3,  a4, a5, a6, a7, s2, s3, s4, s5, s6, s7, s8]>>>,
  
  //Always assign the sret pointer to the first arg reg
  CCIfSRet<CCAssignToReg<[a0]>>,
  //Promote small int types to i32
  CCIfType<[i8,i16], CCPromoteToType<i32>>,

  // Other arguments are passed in 8-byte-aligned 8-byte stack slots.
  CCIfType<[i32, i64, f32, f64], CCAssignToStack<8, 8>>
]>;

//Var args are all passed in integer regs
def CC_REDEFINE32_VAR : CallingConv<[

  
  CCIfInReg<CCIfType<[i32, i64], CCAssignToReg<	
                   [ra, sp, gp, tp,  t0, t1, t2, t3, fp, s0, s1, a0, a1, a2, a3, a4]>>>,

//Always assign the sret pointer to the first arg reg
  CCIfSRet<CCAssignToReg<[a0]>>,
  //Promote small int types to i32
  CCIfType<[i8,i16], CCPromoteToType<i32>>,


  // Other arguments are passed in 8-byte-aligned 8-byte stack slots.
  CCIfType<[i32, i64, f32, f64], CCAssignToStack<8, 8>>
]>;

def CSR_REDEFINE32  : CalleeSavedRegs<(add ra, sp, tp, gp, (sequence "s%u", 11, 0))>;

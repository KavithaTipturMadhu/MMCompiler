//===-- REDEFINEInstrInfo.h - REDEFINE instruction information ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the REDEFINE implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_REDEFINEINSTRINFO_H
#define LLVM_TARGET_REDEFINEINSTRINFO_H

#include "REDEFINE.h"
#include "REDEFINERegisterInfo.h"
#include "llvm/Target/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "REDEFINEGenInstrInfo.inc"

namespace llvm {

class REDEFINETargetMachine;

namespace REDEFINEII {
  enum {
    // See comments in REDEFINEInstrFormats.td.
    SimpleLoad  = (1 << 0),
    SimpleStore = (1 << 1)
  };
  // REDEFINE MachineOperand target flags.
  enum {
    // Masks out the bits for the access model.
    MO_SYMBOL_MODIFIER = (1 << 0),

    // @GOT (aka @GOTENT)
    MO_GOT = (1 << 0),

//    MO_ABS_HI,
//    MO_ABS_LO,
//    MO_TPREL_HI,
//    MO_TPREL_LO
  };
}

class REDEFINEInstrInfo : public REDEFINEGenInstrInfo {
  const REDEFINERegisterInfo RI;
  const REDEFINETargetMachine &TM;

  void splitMove(MachineBasicBlock::iterator MI, unsigned NewOpcode) const;
  void splitAdjDynAlloc(MachineBasicBlock::iterator MI) const;

public:
  explicit REDEFINEInstrInfo(REDEFINETargetMachine &TM);

  // Override TargetInstrInfo.
  unsigned GetInstSizeInBytes(MachineInstr *I) const;
  virtual bool AnalyzeBranch(MachineBasicBlock &MBB,
                             MachineBasicBlock *&TBB,
                             MachineBasicBlock *&FBB,
                             SmallVectorImpl<MachineOperand> &Cond,
                             bool AllowModify) const LLVM_OVERRIDE;
  virtual unsigned RemoveBranch(MachineBasicBlock &MBB) const LLVM_OVERRIDE;
  virtual unsigned InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                                MachineBasicBlock *FBB,
                                const SmallVectorImpl<MachineOperand> &Cond,
                                DebugLoc DL) const LLVM_OVERRIDE;
  virtual unsigned InsertBranchAtInst(MachineBasicBlock &MBB, MachineInstr *I,
                                MachineBasicBlock *TBB,
                                const SmallVectorImpl<MachineOperand> &Cond,
                                DebugLoc DL) const;
  virtual unsigned InsertConstBranchAtInst(MachineBasicBlock &MBB, MachineInstr *I,
                                int64_t offset,
                                const SmallVectorImpl<MachineOperand> &Cond,
                                DebugLoc DL) const;
  virtual void copyPhysReg(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MBBI, DebugLoc DL,
                           unsigned DestReg, unsigned SrcReg,
                           bool KillSrc) const LLVM_OVERRIDE;
  virtual bool
    expandPostRAPseudo(MachineBasicBlock::iterator MBBI) const LLVM_OVERRIDE;
  virtual bool
    ReverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const
    LLVM_OVERRIDE;

  // Return the REDEFINERegisterInfo, which this class owns.
  const REDEFINERegisterInfo &getRegisterInfo() const { return RI; }

  // Return true if MI is a conditional or unconditional branch.
  // When returning true, set Cond to the mask of condition-code
  // values on which the instruction will branch, and set Target
  // to the operand that contains the branch target.  This target
  // can be a register or a basic block.
  bool isBranch(const MachineInstr *MI, SmallVectorImpl<MachineOperand> &Cond,
                const MachineOperand *&Target) const;

  // Get the load and store opcodes for a given register class.
  void getLoadStoreOpcodes(const TargetRegisterClass *RC,
                           unsigned &LoadOpcode, unsigned &StoreOpcode) const;

  // Opcode is the opcode of an instruction that has an address operand,
  // and the caller wants to perform that instruction's operation on an
  // address that has displacement Offset.  Return the opcode of a suitable
  // instruction (which might be Opcode itself) or 0 if no such instruction
  // exists.
  unsigned getOpcodeForOffset(unsigned Opcode, int64_t Offset) const;

};
} // end namespace llvm

#endif

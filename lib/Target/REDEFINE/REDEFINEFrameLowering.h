//===-- REDEFINEFrameLowering.h - Frame lowering for REDEFINE -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef REDEFINEFRAMELOWERING_H
#define REDEFINEFRAMELOWERING_H

#include "REDEFINESubtarget.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
class REDEFINETargetMachine;
class REDEFINESubtarget;

class REDEFINEFrameLowering : public TargetFrameLowering {
protected:
  const REDEFINETargetMachine &TM;
  const REDEFINESubtarget &STI;

public:
  REDEFINEFrameLowering(const REDEFINETargetMachine &tm,
                       const REDEFINESubtarget &sti);

  static const REDEFINEFrameLowering *create(REDEFINETargetMachine &TM,
                                         const REDEFINESubtarget &ST);

  bool hasFP(const MachineFunction &MF) const;

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF) const;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const;

  void eliminateCallFramePseudoInstr(MachineFunction &MF,
                                     MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I) const;

  bool spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MI,
                                 const std::vector<CalleeSavedInfo> &CSI,
                                 const TargetRegisterInfo *TRI) const;

  bool hasReservedCallFrame(const MachineFunction &MF) const;

  void processFunctionBeforeCalleeSavedScan(MachineFunction &MF,
                                            RegScavenger *RS) const;
  unsigned ehDataReg(unsigned I) const;
};

} // End llvm namespace

#endif

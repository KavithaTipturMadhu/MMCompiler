//===-- REDEFINEMCTargetDesc.h - RISCV target descriptions -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef REDEFINEMCTARGETDESC_H
#define REDEFINEMCTARGETDESC_H

#include "llvm/Support/DataTypes.h"

namespace llvm {

class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class StringRef;
class Target;
class raw_ostream;

extern Target TheREDEFINETarget;

namespace REDEFINEMC {
//	TODO Left this so that nothing breaks, not required
  // How many bytes are in the ABI-defined, caller-allocated part of
  // a stack frame.
  const int64_t CallFrameSize = 160;

  // The offset of the DWARF CFA from the incoming stack pointer.
  const int64_t CFAOffsetFromInitialSP = CallFrameSize;
}

MCCodeEmitter *createREDEFINEMCCodeEmitter(const MCInstrInfo &MCII,
                                          const MCRegisterInfo &MRI,
                                          const MCSubtargetInfo &STI,
                                          MCContext &Ctx);

MCAsmBackend *createREDEFINEMCAsmBackend(const Target &T, StringRef TT,
                                        StringRef CPU);

MCObjectWriter *createREDEFINEObjectWriter(raw_ostream &OS, uint8_t OSABI);
} // end namespace llvm

// Defines symbolic names for REDEFINE registers.
// This defines a mapping from register name to register number.
#define GET_REGINFO_ENUM
#include "REDEFINEGenRegisterInfo.inc"

// Defines symbolic names for the REDEFINE instructions.
#define GET_INSTRINFO_ENUM
#include "REDEFINEGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "REDEFINEGenSubtargetInfo.inc"

#endif

//===- REDEFINEConstantPoolValue.h - REDEFINE constant-pool value -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef REDEFINECONSTANTPOOLVALUE_H
#define REDEFINECONSTANTPOOLVALUE_H

#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

class GlobalValue;

namespace REDEFINECP {
  enum REDEFINECPModifier {
    NTPOFF
  };
}

/// A REDEFINE-specific constant pool value.  At present, the only
/// defined constant pool values are offsets of thread-local variables
/// (written x@NTPOFF).
class REDEFINEConstantPoolValue : public MachineConstantPoolValue {
  const GlobalValue *GV;
  REDEFINECP::REDEFINECPModifier Modifier;

protected:
  REDEFINEConstantPoolValue(const GlobalValue *GV,
                           REDEFINECP::REDEFINECPModifier Modifier);

public:
  static REDEFINEConstantPoolValue *
    Create(const GlobalValue *GV, REDEFINECP::REDEFINECPModifier Modifier);

  // Override MachineConstantPoolValue.
  virtual unsigned getRelocationInfo() const LLVM_OVERRIDE;
  virtual int getExistingMachineCPValue(MachineConstantPool *CP,
                                        unsigned Alignment) LLVM_OVERRIDE;
  virtual void addSelectionDAGCSEId(FoldingSetNodeID &ID) LLVM_OVERRIDE;
  virtual void print(raw_ostream &O) const LLVM_OVERRIDE;

  // Access REDEFINE-specific fields.
  const GlobalValue *getGlobalValue() const { return GV; }
  REDEFINECP::REDEFINECPModifier getModifier() const { return Modifier; }
};

} // End llvm namespace

#endif

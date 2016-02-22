//===-- REDEFINEConstantPoolValue.cpp - REDEFINE constant-pool value --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "REDEFINEConstantPoolValue.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

REDEFINEConstantPoolValue::
REDEFINEConstantPoolValue(const GlobalValue *gv,
                         REDEFINECP::REDEFINECPModifier modifier)
  : MachineConstantPoolValue(gv->getType()), GV(gv), Modifier(modifier) {}

REDEFINEConstantPoolValue *
REDEFINEConstantPoolValue::Create(const GlobalValue *GV,
                                 REDEFINECP::REDEFINECPModifier Modifier) {
  return new REDEFINEConstantPoolValue(GV, Modifier);
}

unsigned REDEFINEConstantPoolValue::getRelocationInfo() const {
  switch (Modifier) {
  case REDEFINECP::NTPOFF:
    // May require a relocation, but the relocations are always resolved
    // by the static linker.
    return 1;
  }
  llvm_unreachable("Unknown modifier");
}

int REDEFINEConstantPoolValue::
getExistingMachineCPValue(MachineConstantPool *CP, unsigned Alignment) {
  unsigned AlignMask = Alignment - 1;
  const std::vector<MachineConstantPoolEntry> Constants = CP->getConstants();
  for (unsigned I = 0, E = Constants.size(); I != E; ++I) {
    if (Constants[I].isMachineConstantPoolEntry() &&
        (Constants[I].getAlignment() & AlignMask) == 0) {
      REDEFINEConstantPoolValue *RCPV =
        static_cast<REDEFINEConstantPoolValue *>(Constants[I].Val.MachineCPVal);
      if (RCPV->GV == GV && RCPV->Modifier == Modifier)
        return I;
    }
  }
  return -1;
}

void REDEFINEConstantPoolValue::addSelectionDAGCSEId(FoldingSetNodeID &ID) {
  ID.AddPointer(GV);
  ID.AddInteger(Modifier);
}

void REDEFINEConstantPoolValue::print(raw_ostream &O) const {
  O << GV ;//<< "@" << int(Modifier);
}

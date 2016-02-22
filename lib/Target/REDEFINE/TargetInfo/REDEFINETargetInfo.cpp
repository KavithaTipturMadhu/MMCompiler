//===-- RISCVTargetInfo.cpp - RISCV target implementation -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "REDEFINE.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

Target llvm::TheREDEFINETarget;

extern "C" void LLVMInitializeREDEFINETargetInfo() {
  RegisterTarget<Triple::redefine, /*HasJIT=*/false>
    X(TheREDEFINETarget, "redefine", "REDEFINE");
}

//====-- REDEFINEMCAsmInfo.h - RISCV asm properties -----------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef REDEFINETARGETASMINFO_H
#define REDEFINETARGETASMINFO_H

#include "llvm/MC/MCAsmInfo.h"
#include "llvm/Support/Compiler.h"

namespace llvm {
class Target;
class StringRef;

class REDEFINEMCAsmInfo : public MCAsmInfo {
public:
  explicit REDEFINEMCAsmInfo(const Target &T, StringRef TT);

  // Override MCAsmInfo;
  virtual const MCSection *getNonexecutableStackSection(MCContext &Ctx) const
    LLVM_OVERRIDE;
};

} // namespace llvm

#endif

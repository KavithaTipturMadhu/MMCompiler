//===-- REDEFINESubtarget.cpp - REDEFINE subtarget information --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "REDEFINESubtarget.h"
#include "REDEFINE.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/Support/Debug.h"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "REDEFINEGenSubtargetInfo.inc"

using namespace llvm;

REDEFINESubtarget::REDEFINESubtarget(const std::string &TT,
                                   const std::string &CPU,
                                   const std::string &FS)
  : REDEFINEGenSubtargetInfo(TT, CPU, FS), TargetTriple(TT),
    REDEFINEArchVersion(REDEFINE32), CECount(1), M(1), N(1),DGM(65536),L1(32),CFCount(52),CFSize(16)
{
  // Parse features string.
  std::string CPUName = CPU;
  if (CPUName.empty())
    CPUName = "REDEFINE";
  ParseSubtargetFeatures(CPUName, FS);

  // Only use instruction scheduling if the selected CPU has an instruction
  // itinerary (the default CPU is the only one that doesn't).
  HasItin = CPUName != "REDEFINE";
  DEBUG(dbgs() << "CPU " << CPUName << "(" << HasItin << ")\n");

  // Initialize scheduling itinerary for the specified CPU.
  InstrItins = getInstrItineraryForCPU(CPUName);
}

// Return true if GV binds locally under reloc model RM.
static bool bindsLocally(const GlobalValue *GV, Reloc::Model RM) {
  // For non-PIC, all symbols bind locally.
  if (RM == Reloc::Static)
    return true;

  return GV->hasLocalLinkage() || !GV->hasDefaultVisibility();
}

bool REDEFINESubtarget::isPC32DBLSymbol(const GlobalValue *GV,
                                       Reloc::Model RM,
                                       CodeModel::Model CM) const {
  // PC32DBL accesses require the low bit to be clear.  Note that a zero
  // value selects the default alignment and is therefore OK.
  if (GV->getAlignment() == 1)
    return false;

  // For the small model, all locally-binding symbols are in range.
  if (CM == CodeModel::Small)
    return bindsLocally(GV, RM);

  // For Medium and above, assume that the symbol is not within the 4GB range.
  // Taking the address of locally-defined text would be OK, but that
  // case isn't easy to detect.
  return false;
}

bool REDEFINESubtarget::enableMachineScheduler() const{
	return true;
}

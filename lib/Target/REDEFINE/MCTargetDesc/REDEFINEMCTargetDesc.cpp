//===-- RISCVMCTargetDesc.cpp - RISCV target descriptions -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "REDEFINEMCTargetDesc.h"
#include "InstPrinter/REDEFINEInstPrinter.h"
#include "REDEFINEMCAsmInfo.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "REDEFINEGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "REDEFINEGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "REDEFINEGenRegisterInfo.inc"

using namespace llvm;

static MCAsmInfo *createREDEFINEMCAsmInfo(const Target &T, StringRef TT) {
  MCAsmInfo *MAI = new REDEFINEMCAsmInfo(T, TT);
  MachineLocation FPDst(MachineLocation::VirtualFP);
  MachineLocation FPSrc(REDEFINE::sp, -REDEFINEMC::CFAOffsetFromInitialSP);
  MAI->addInitialFrameState(0, FPDst, FPSrc);
  return MAI;
}

static MCInstrInfo *createREDEFINEMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitREDEFINEMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createREDEFINEMCRegisterInfo(StringRef TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitREDEFINEMCRegisterInfo(X, REDEFINE::sp);
  return X;
}

static MCSubtargetInfo *createREDEFINEMCSubtargetInfo(StringRef TT,
                                                     StringRef CPU,
                                                     StringRef FS) {
  MCSubtargetInfo *X = new MCSubtargetInfo();
  InitREDEFINEMCSubtargetInfo(X, TT, CPU, FS);
  return X;
}

static MCCodeGenInfo *createREDEFINEMCCodeGenInfo(StringRef TT, Reloc::Model RM,
                                                 CodeModel::Model CM,
                                                 CodeGenOpt::Level OL) {
  MCCodeGenInfo *X = new MCCodeGenInfo();

  // Static code is suitable for use in a dynamic executable; there is no
  // separate DynamicNoPIC model.
  if (RM == Reloc::Default || RM == Reloc::DynamicNoPIC)
    RM = Reloc::Static;

  // For RISCV we define the models as follows:
  //
  // Small:  BRASL can call any function and will use a stub if necessary.
  //         Locally-binding symbols will always be in range of LARL.
  //
  // Medium: BRASL can call any function and will use a stub if necessary.
  //         GOT slots and locally-defined text will always be in range
  //         of LARL, but other symbols might not be.
  //
  // Large:  Equivalent to Medium for now.
  //
  // Kernel: Equivalent to Medium for now.
  //
  // This means that any PIC module smaller than 4GB meets the
  // requirements of Small, so Small seems like the best default there.
  //
  // All symbols bind locally in a non-PIC module, so the choice is less
  // obvious.  There are two cases:
  //
  // - When creating an executable, PLTs and copy relocations allow
  //   us to treat external symbols as part of the executable.
  //   Any executable smaller than 4GB meets the requirements of Small,
  //   so that seems like the best default.
  //
  // - When creating JIT code, stubs will be in range of BRASL if the
  //   image is less than 4GB in size.  GOT entries will likewise be
  //   in range of LARL.  However, the JIT environment has no equivalent
  //   of copy relocs, so locally-binding data symbols might not be in
  //   the range of LARL.  We need the Medium model in that case.
  if (CM == CodeModel::Default)
    CM = CodeModel::Small;
  else if (CM == CodeModel::JITDefault)
    CM = RM == Reloc::PIC_ ? CodeModel::Small : CodeModel::Medium;
  X->InitMCCodeGenInfo(RM, CM, OL);
  return X;
}

static MCInstPrinter *createREDEFINEMCInstPrinter(const Target &T,
                                                 unsigned SyntaxVariant,
                                                 const MCAsmInfo &MAI,
                                                 const MCInstrInfo &MII,
                                                 const MCRegisterInfo &MRI,
                                                 const MCSubtargetInfo &STI) {
  return new REDEFINEInstPrinter(MAI, MII, MRI);
}

static MCStreamer *createREDEFINEMCObjectStreamer(const Target &T, StringRef TT,
                                                 MCContext &Ctx,
                                                 MCAsmBackend &MAB,
                                                 raw_ostream &OS,
                                                 MCCodeEmitter *Emitter,
                                                 bool RelaxAll,
                                                 bool NoExecStack) {
  return createELFStreamer(Ctx, MAB, OS, Emitter, RelaxAll, NoExecStack);
}

extern "C" void LLVMInitializeREDEFINETargetMC() {
  // Register the MCAsmInfo.
  TargetRegistry::RegisterMCAsmInfo(TheREDEFINETarget,
                                    createREDEFINEMCAsmInfo);

  // Register the MCCodeGenInfo.
  TargetRegistry::RegisterMCCodeGenInfo(TheREDEFINETarget,
                                        createREDEFINEMCCodeGenInfo);

  // Register the MCCodeEmitter.
  TargetRegistry::RegisterMCCodeEmitter(TheREDEFINETarget,
		  createREDEFINEMCCodeEmitter);

  // Register the MCInstrInfo.
  TargetRegistry::RegisterMCInstrInfo(TheREDEFINETarget,
                                      createREDEFINEMCInstrInfo);

  // Register the MCRegisterInfo.
  TargetRegistry::RegisterMCRegInfo(TheREDEFINETarget,
                                    createREDEFINEMCRegisterInfo);

  // Register the MCSubtargetInfo.
  TargetRegistry::RegisterMCSubtargetInfo(TheREDEFINETarget,
                                          createREDEFINEMCSubtargetInfo);

  // Register the MCAsmBackend.
  TargetRegistry::RegisterMCAsmBackend(TheREDEFINETarget,
		  createREDEFINEMCAsmBackend);

  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(TheREDEFINETarget,
                                        createREDEFINEMCInstPrinter);

  // Register the MCObjectStreamer;
  TargetRegistry::RegisterMCObjectStreamer(TheREDEFINETarget,
                                           createREDEFINEMCObjectStreamer);
}

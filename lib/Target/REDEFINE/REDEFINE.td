//===-- REDEFINE.td - Describe the REDEFINE target machine -----*- tblgen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//
include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// REDEFINE subtarget features
//===----------------------------------------------------------------------===//

def FeatureCECount4 : SubtargetFeature<"has4ce", "CECount", "4",
                                "Supports 4 CEs per CR, default is 1">;
def FeatureFabricMDim4 : SubtargetFeature<"has4rows", "M", "4",
                                "Supports 4 CRs along each row, default is 1">;      
def FeatureFabricNDim4 : SubtargetFeature<"has4columns", "N", "4",
                                "Supports 4 columns of CRs, default is 1">;        
def FeatureFabricMDim3 : SubtargetFeature<"has3rows", "M", "3",
                                "Supports 3 CRs along each row, default is 1">;      
def FeatureFabricNDim3 : SubtargetFeature<"has3columns", "N", "3",
                                "Supports 3 columns of CRs, default is 1">;    
                                
def FeatureFabricMDim2 : SubtargetFeature<"has2rows", "M", "2",
                                "Supports 2 CRs along each row, default is 1">;      
def FeatureFabricNDim2 : SubtargetFeature<"has2columns", "N", "2",
                                "Supports 2 columns of CRs, default is 1">;       

                                
class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, NoItineraries, Features>{
}

def : Proc<"REDEFINE", [FeatureCECount4, FeatureFabricMDim4, FeatureFabricNDim4, FeatureFabricMDim3, FeatureFabricNDim3]>;

//===----------------------------------------------------------------------===//
// Register file description
//===----------------------------------------------------------------------===//

include "REDEFINERegisterInfo.td"

//===----------------------------------------------------------------------===//
// Calling convention description
//===----------------------------------------------------------------------===//

include "REDEFINECallingConv.td"

//===----------------------------------------------------------------------===//
// Instruction descriptions
//===----------------------------------------------------------------------===//

include "REDEFINEOperators.td"
include "REDEFINEOperands.td"
include "REDEFINEInstrFormats.td"
include "REDEFINEInstrInfo.td"


def REDEFINEInstrInfo : InstrInfo {}

//===----------------------------------------------------------------------===//
// Assembly parser
//===----------------------------------------------------------------------===//

def REDEFINEAsmParser : AsmParser {
  let ShouldEmitMatchRegisterName = 0;
}

//===----------------------------------------------------------------------===//
// Assembly writer
//===----------------------------------------------------------------------===//

def REDEFINEAsmWriter : AsmWriter {
  string AsmWriterClassName = "InstPrinter";
  bit isMCAsmWriter = 1;
}

//===----------------------------------------------------------------------===//
// Top-level target declaration
//===----------------------------------------------------------------------===//

def REDEFINE : Target {
  let InstructionSet = REDEFINEInstrInfo;
  let AssemblyParsers = [REDEFINEAsmParser];
  let AssemblyWriters = [REDEFINEAsmWriter];
}

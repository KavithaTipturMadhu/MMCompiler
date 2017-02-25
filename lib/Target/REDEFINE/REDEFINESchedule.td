//===-- REDEFINESchedule.td - REDEFINE Scheduling Definitions ------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Functional units across REDEFINE chips sets. Based on GCC/REDEFINE backend files.
//===----------------------------------------------------------------------===//
def IF       : FuncUnit;
def ID       : FuncUnit;
def EXE      : FuncUnit;
def FPXA      : FuncUnit;
def FPXB      : FuncUnit;
def FPXC      : FuncUnit;
def MEM      : FuncUnit;
def WB       : FuncUnit;


//===----------------------------------------------------------------------===//
// Instruction Itinerary classes used for REDEFINE
//===----------------------------------------------------------------------===//
// REDEFINEInt is a placeholder class for most REDEFINE16 instructions.
def REDEFINEALU           : InstrItinClass;
def REDEFINEBR            : InstrItinClass;
def REDEFINELD            : InstrItinClass;
def REDEFINEST            : InstrItinClass;

def REDEFINEORC           : InstrItinClass;
def REDEFINESPM           : InstrItinClass;

def REDEFINEFPA           : InstrItinClass;
def REDEFINEFPB           : InstrItinClass;
def REDEFINEFPC           : InstrItinClass;

//===----------------------------------------------------------------------===//
// REDEFINE Generic instruction itineraries.
//===----------------------------------------------------------------------===//
def REDEFINEItineraries : ProcessorItineraries<[IF, ID, EXE , MEM , WB, FPXA, FPXB, FPXC], [], [
InstrItinData<REDEFINEALU,[InstrStage<1,[IF]>,InstrStage<1,[ID]>,InstrStage<1,  [EXE]>,InstrStage<0,[MEM]>, InstrStage<1,[WB]>],[4,0,0]>,
InstrItinData<REDEFINEBR, [InstrStage<1,[IF]>,InstrStage<1,[ID]>,InstrStage<1,  [EXE]>,InstrStage<0,[MEM]>, InstrStage<0,[WB]>],[3,0,0]>,
InstrItinData<REDEFINELD, [InstrStage<1,[IF]>,InstrStage<1,[ID]>,InstrStage<1,  [EXE]>,InstrStage<10,[MEM]>,InstrStage<1,[WB]>],[14,0,0]>,
InstrItinData<REDEFINEST, [InstrStage<1,[IF]>,InstrStage<1,[ID]>,InstrStage<1,  [EXE]>,InstrStage<10,[MEM]>,InstrStage<0,[WB]>],[13,0,0]>,
InstrItinData<REDEFINEFPA,[InstrStage<1,[IF]>,InstrStage<1,[ID]>,InstrStage<15,[FPXA]>,InstrStage<0,[MEM]>, InstrStage<1,[WB]>],[18,0,0]>,
InstrItinData<REDEFINEFPB,[InstrStage<1,[IF]>,InstrStage<1,[ID]>,InstrStage<15,[FPXB]>,InstrStage<0,[MEM]>, InstrStage<1,[WB]>],[18,0,0]>,
InstrItinData<REDEFINEFPC,[InstrStage<1,[IF]>,InstrStage<1,[ID]>,InstrStage<15,[FPXC]>,InstrStage<0,[MEM]>, InstrStage<1,[WB]>],[18,0,0]>,
InstrItinData<REDEFINEORC,[InstrStage<1,[IF]>,InstrStage<1,[ID]>,InstrStage<1,  [EXE]>,InstrStage<0,[MEM]>, InstrStage<0,[WB]>],[3,0,0]>,
InstrItinData<REDEFINESPM,[InstrStage<1,[IF]>,InstrStage<1,[ID]>,InstrStage<1,  [EXE]>,InstrStage<0,[MEM]>, InstrStage<0,[WB]>],[3,0,0]>
]>;

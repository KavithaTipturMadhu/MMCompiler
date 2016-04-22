//==- REDEFINERegisterInfo.td - REDEFINE register definitions -*- tablegen -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Class definitions.
//===----------------------------------------------------------------------===//
class REDEFINEReg<string n> : Register<n> {
  let Namespace = "REDEFINE";
}

class REDEFINERegWithSubRegs<string n, list<Register> subregs>
  : RegisterWithSubRegs<n, subregs> {
  let Namespace = "REDEFINE";
}

//Subreg defs
let Namespace = "REDEFINE" in {
  def sub_32       : SubRegIndex;
  def sub_32even   : SubRegIndex;
  def sub_32odd    : SubRegIndex;
}


// Define a register class that contains values of type TYPE and an
// associated operand called NAME.  SIZE is the size and alignment
// of the registers and REGLIST is the list of individual registers.
multiclass REDEFINERegClass<string name, ValueType type, int size, dag regList, bit allocatable> {
  def AsmOperand : AsmOperandClass {
    let Name = name;
    let ParserMethod = "parse"##name;
    let RenderMethod = "addRegOperands";
  }
  def Bit : RegisterClass<"REDEFINE", [type], size, regList> {
    let Size = size;
    let isAllocatable = allocatable;
  }
  def "" : RegisterOperand<!cast<RegisterClass>(name##"Bit")> {
    let ParserMatchClass = !cast<AsmOperandClass>(name##"AsmOperand");
  }
}

//===----------------------------------------------------------------------===//
// General-purpose registers
//===----------------------------------------------------------------------===//

//RV32 general purpose regs
class GPR32<bits<16> num, string n> : REDEFINEReg<n> {
  let HWEncoding = num;
}

// General-purpose registers
//ABI Names
def zero : GPR32<0, "x0">, DwarfRegNum<[0]>;
def ra   : GPR32<1, "x1">, DwarfRegNum<[1]>;
def sp   : GPR32<2, "x2">, DwarfRegNum<[2]>;
def gp   : GPR32<3, "x3">, DwarfRegNum<[3]>;
def tp   : GPR32<4, "x4">, DwarfRegNum<[4]>;
def t0   : GPR32<5, "x5">, DwarfRegNum<[5]>;
def t1   : GPR32<6, "x6">, DwarfRegNum<[6]>;
def t2   : GPR32<7, "x7">, DwarfRegNum<[7]>;
def fp   : GPR32<8, "x8">, DwarfRegNum<[8]>;
def s0   : GPR32<9, "x9">, DwarfRegNum<[9]>;
def s1   : GPR32<10, "x10">, DwarfRegNum<[10]>;
//return values
def a0   : GPR32<11,"x11">, DwarfRegNum<[11]>;
def a1   : GPR32<12,"x12">, DwarfRegNum<[12]>;
//function arguments
def a2   : GPR32<13,"x13">, DwarfRegNum<[13]>;
def a3   : GPR32<14,"x14">, DwarfRegNum<[14]>;
def a4   : GPR32<15,"x15">, DwarfRegNum<[15]>;
def a5   : GPR32<16,"x16">, DwarfRegNum<[16]>;
def a6   : GPR32<17,"x17">, DwarfRegNum<[17]>;
def a7   : GPR32<18,"x18">, DwarfRegNum<[18]>;
//saved registers
def s2   : GPR32<19,"x19">, DwarfRegNum<[19]>;
def s3   : GPR32<20,"x20">, DwarfRegNum<[20]>;
def s4   : GPR32<21,"x21">, DwarfRegNum<[21]>;
def s5   : GPR32<22,"x22">, DwarfRegNum<[22]>;
def s6   : GPR32<23,"x23">, DwarfRegNum<[23]>;
def s7   : GPR32<24,"x24">, DwarfRegNum<[24]>;
def s8   : GPR32<25,"x25">, DwarfRegNum<[25]>;
def s9   : GPR32<26,"x26">, DwarfRegNum<[26]>;
def s10  : GPR32<27,"x27">, DwarfRegNum<[27]>;
def s11  : GPR32<28,"x28">, DwarfRegNum<[28]>;
//temp registers
def t3   : GPR32<29,"x29">, DwarfRegNum<[29]>;
def t4   : GPR32<30,"x30">, DwarfRegNum<[30]>;
def t5   : GPR32<31,"x31">, DwarfRegNum<[31]>;

//Integer regs 32bit
defm GR32 : REDEFINERegClass<"GR32", i32, 32, (add
  zero, ra, sp, gp, tp,
  t0, t1, t2,
  fp, s0, s1,
  a0, a1, a2, a3, a4, a5, a6, a7, 
  s2, s3, s4, s5, s6, s7, s8, s9, s10, s11,
  t3, t4, t5), 1>;

//===----------------------------------------------------------------------===//
// Floating-point registers
//===----------------------------------------------------------------------===//

// Lower 32 bits of one of the 16 64-bit floating-point registers
class FPR32<bits<16> num, string n> : REDEFINEReg<n> {
  let HWEncoding = num;
}


// Floating-point registers
//ABI Names
//FP temporary registers
def ft0 : FPR32<0, "f0">, DwarfRegNum<[0]>;
def ft1 : FPR32<1, "f1">, DwarfRegNum<[1]>;
def ft2 : FPR32<2, "f2">, DwarfRegNum<[2]>;
def ft3 : FPR32<3, "f3">, DwarfRegNum<[3]>;
def ft4 : FPR32<4, "f4">, DwarfRegNum<[4]>;
def ft5 : FPR32<5, "f5">, DwarfRegNum<[5]>;
def ft6 : FPR32<6, "f6">, DwarfRegNum<[6]>;
def ft7 : FPR32<7, "f7">, DwarfRegNum<[7]>;

def fs0 : FPR32<8, "f8">, DwarfRegNum<[8]>;
def fs1 : FPR32<9, "f9">, DwarfRegNum<[9]>;
//FP arguments
def fa0 : FPR32<10,"f10">, DwarfRegNum<[10]>;
def fa1 : FPR32<11,"f11">, DwarfRegNum<[11]>;
def fa2 : FPR32<12,"f12">, DwarfRegNum<[12]>;
def fa3 : FPR32<13,"f13">, DwarfRegNum<[13]>;
def fa4 : FPR32<14,"f14">, DwarfRegNum<[14]>;
def fa5 : FPR32<15,"f15">, DwarfRegNum<[15]>;
def fa6 : FPR32<16,"f16">, DwarfRegNum<[16]>;
def fa7 : FPR32<17,"f17">, DwarfRegNum<[17]>;
//FP saved registers
def fs2  : FPR32<18,"f18">, DwarfRegNum<[18]>;
def fs3  : FPR32<19,"f19">, DwarfRegNum<[19]>;
def fs4  : FPR32<20,"f20">, DwarfRegNum<[20]>;
def fs5  : FPR32<21,"f21">, DwarfRegNum<[21]>;
def fs6  : FPR32<22,"f22">, DwarfRegNum<[22]>;
def fs7  : FPR32<23,"f23">, DwarfRegNum<[23]>;
def fs8  : FPR32<24,"f24">, DwarfRegNum<[24]>;
def fs9  : FPR32<25,"f25">, DwarfRegNum<[25]>;
def fs10 : FPR32<26,"f26">, DwarfRegNum<[26]>;
def fs11 : FPR32<27,"f27">, DwarfRegNum<[27]>;
//FP temp registers
def ft8 : FPR32<28,"f28">, DwarfRegNum<[28]>;
def ft9 : FPR32<29,"f29">, DwarfRegNum<[29]>;
def ft10: FPR32<30,"f30">, DwarfRegNum<[30]>;
def ft11: FPR32<31,"f31">, DwarfRegNum<[31]>;

//all fp regs are 32bit
defm FP32  : REDEFINERegClass<"FP32", f32, 32, (add
  ft0, ft1, ft2, ft3, ft4, ft5, ft6, ft7,
  fs0, fs1, 
  fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7,
  fs2, fs3, fs4, fs5, fs6, fs7, fs8, fs9, fs10, fs11,
  ft8, ft9, ft10, ft11
  ), 1>;


//===-- REDEFINEInstrInfo.td - General REDEFINE instructions ----*- tblgen-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

/*******************
*REDEFINE Instructions
********************/
//Upper Immediate
def LUI: InstU<0b0110111, (outs GR32:$dst), (ins imm32sxu20:$imm),
               "lui\t$dst, $imm",
               [(set GR32:$dst, (shl imm32sx20:$imm, (i32 12)))]>;

def AUIPC: InstU<0b0110111, (outs GR32:$dst), (ins pcimm:$target),
               "auipc\t$dst, $target",
               [(set GR32:$dst, (r_pcrel_wrapper tglobaladdr:$target))]>;
 
//Load/Store Instructions
let mayLoad = 1 in {
  def LW : InstLoad <"lw" , 0b0000011, 0b010, load, GR32, mem>; 
  def LH : InstLoad <"lw" , 0b0000011, 0b010, sextloadi16, GR32, mem>; 
  def LHU: InstLoad <"lw", 0b0000011, 0b010, zextloadi16, GR32, mem>; 
  def LB : InstLoad <"lw" , 0b0000011, 0b010, sextloadi8, GR32, mem>; 
  def LBU: InstLoad <"lw", 0b0000011, 0b010, zextloadi8, GR32, mem>; 
  def FLW: InstLoad <"flw", 0b0000011, 0b100, loadf32, FP32, mem>; 
}

//extended loads
def : Pat<(i32 (extloadi1  addr:$addr)), (LBU addr:$addr)>;
def : Pat<(i32 (extloadi8  addr:$addr)), (LBU addr:$addr)>;
def : Pat<(i32 (extloadi16 addr:$addr)), (LHU addr:$addr)>;

let mayStore = 1 in {
// SW is of the format <sw rs1, rs2, imm> translating to imm(rs1)<-rs2
  def SW : InstStore<"sw" , 0b0100011, 0b010, store, GR32>;
  def SH : InstStore<"sw" , 0b0100011, 0b010, truncstorei16, GR32>; 
  def SB : InstStore<"sw" , 0b0100011, 0b010, truncstorei8 , GR32>; 
  def FSW : InstStore <"fsw" , 0b0100111, 0b010, store, FP32>; 
}


//Integer arithmetic register-register
def ADDI: InstI<"addi", 0b0010011, 0b000       , add, GR32, GR32, imm32sx12>{
	//Hack to support > 12 bit immediates
	let isPseudo = 1;
}
//Integer arithmetic register-immediate
def XORI: InstI<"xori", 0b0010011, 0b100       , xor, GR32, GR32, imm32sx12>;
def ORI : InstI<"ori" , 0b0010011, 0b110       , or , GR32, GR32, imm32sx12>;
def ANDI: InstI<"andi", 0b0010011, 0b111       , and, GR32, GR32, imm32sx12>;

def SLTI : InstI<"slti", 0b0010011, 0b010, setlt, GR32, GR32, imm32sx12>;
def SLTIU: InstI<"sltiu",0b0010011, 0b011, setult,GR32, GR32, imm32sx12>;

def ADD : InstR<"add" , 0b0110011, 0b0000000000, add   , GR32, GR32>;
def SUB : InstR<"sub" , 0b0110011, 0b1000000000, sub   , GR32, GR32>;
def SLL : InstR<"sll" , 0b0110011, 0b1000000001, shl   , GR32, GR32>;
def SLT : InstR<"slt" , 0b0110011, 0b0000000010, setlt , GR32, GR32>;
def SLTU: InstR<"sltu", 0b0110011, 0b0000000011, setult, GR32, GR32>;
def XOR : InstR<"xor" , 0b0110011, 0b0000000100, xor   , GR32, GR32>;
def SRL : InstR<"srl" , 0b0110011, 0b0000000101, srl   , GR32, GR32>;
def SRA : InstR<"sra" , 0b0110011, 0b1000000101, sra   , GR32, GR32>;
def OR  : InstR<"or"  , 0b0110011, 0b0000000110, or    , GR32, GR32>;
def AND : InstR<"and" , 0b0110011, 0b0000000111, and   , GR32, GR32>;

//Unconditional Jumps
let isBranch = 1, isTerminator = 1, isBarrier = 1 in {
  def JAL  : InstJ<0b1100111, (outs), (ins jumptarget:$target), "jal \t$target", 
          [(br bb:$target)]>;
}


//Conditional Branches
//TODO:refactor to class
let isBranch = 1, isTerminator = 1, isBarrier = 1 in {
  def BEQ : InstB<0b1100011, 0b000, (outs), 
              (ins brtarget:$target, GR32:$src1, GR32:$src2), 
              "beq\t$src1, $src2, $target", 
              [(brcond (i32 (seteq GR32:$src1,  GR32:$src2)), bb:$target)]>;
  def BNE : InstB<0b1100011, 0b001, (outs), 
              (ins brtarget:$target, GR32:$src1, GR32:$src2), 
              "bne\t$src1, $src2, $target", 
              [(brcond (i32 (setne GR32:$src1, GR32:$src2)), bb:$target)]>;
  def BLT : InstB<0b1100011, 0b100, (outs), 
              (ins brtarget:$target, GR32:$src1, GR32:$src2), 
              "blt\t$src1, $src2, $target", 
              [(brcond (i32 (setlt GR32:$src1, GR32:$src2)), bb:$target)]>;
  def BGE : InstB<0b1100011, 0b101, (outs), 
              (ins brtarget:$target, GR32:$src1, GR32:$src2), 
              "bge\t$src1, $src2, $target", 
              [(brcond (i32 (setge GR32:$src1, GR32:$src2)), bb:$target)]>;
  def BLTU: InstB<0b1100011, 0b110, (outs), 
              (ins brtarget:$target, GR32:$src1, GR32:$src2), 
              "bltu\t$src1, $src2, $target", 
              [(brcond (i32 (setult GR32:$src1, GR32:$src2)), bb:$target)]>;
  def BGEU: InstB<0b1100011, 0b111, (outs), 
              (ins brtarget:$target, GR32:$src1, GR32:$src2), 
              "bgeu\t$src1, $src2, $target", 
              [(brcond (i32 (setuge GR32:$src1, GR32:$src2)), bb:$target)]>;

//Synthesize remaining condition codes by reverseing operands
  def BGT : InstB<0b1100011, 0b100, (outs), 
              (ins brtarget:$target, GR32:$src1, GR32:$src2), 
              "blt\t$src2, $src1, $target", 
              [(brcond (i32 (setgt GR32:$src1, GR32:$src2)), bb:$target)]>;
  def BGTU: InstB<0b1100011, 0b110, (outs), 
              (ins brtarget:$target, GR32:$src1, GR32:$src2), 
              "bltu\t$src2, $src1, $target", 
              [(brcond (i32 (setugt GR32:$src1, GR32:$src2)), bb:$target)]>;
  def BLE : InstB<0b1100011, 0b101, (outs), 
              (ins brtarget:$target, GR32:$src1, GR32:$src2), 
              "bge\t$src2, $src1, $target", 
              [(brcond (i32 (setle GR32:$src1, GR32:$src2)), bb:$target)]>;
  def BLEU: InstB<0b1100011, 0b111, (outs), 
              (ins brtarget:$target, GR32:$src1, GR32:$src2), 
              "bgeu\t$src2, $src1, $target", 
              [(brcond (i32 (setule GR32:$src1, GR32:$src2)), bb:$target)]>;
}
//constant branches (e.g. br 1 $label or br 0 $label)
def : Pat<(brcond GR32Bit:$cond, bb:$target),
          (BNE bb:$target, GR32Bit:$cond, zero)>;  
def : Pat<(i32 0), (i32 zero)>;
//Conditional moves
// SELECT_CC_* - Used to implement the SELECT_CC DAG operation.  Expanded after
// instruction selection into a branch sequence.
let usesCustomInserter = 1 in {
  def SELECT_CC : Pseudo<(outs GR32:$dst),
                              (ins GR32:$cond, GR32:$T, GR32:$F),
                              [(set GR32:$dst,
                                 (select GR32:$cond, GR32:$T, GR32:$F))]>;
}
def : Pat<(select (i32 (setne GR32:$lhs, 0)), GR32:$T, GR32:$F),
        (SELECT_CC GR32:$lhs, GR32:$T, GR32:$F)>;

def : Pat<(select (i32 (seteq GR32:$lhs, 0)), GR32:$T, GR32:$F),
        (SELECT_CC GR32:$lhs, GR32:$F, GR32:$T)>;

/**
REDEFINE specific instructions
**/
//REDEFINE I instructions 
def CALL: InstIREDEFINE<"call", 0b0000100, 0b000       , add, GR32, GR32, imm32sx12>
{
 let Itinerary   = REDEFINEORC;
}

def FALLOC: InstREDEFINE<4, (outs GR32:$dst), (ins imm32sx12:$imm), 
     "falloc\t$dst, $imm", 
     [(set i32:$dst, (int_falloc imm:$imm))], REDEFINEORC> {
  field bits<32> Inst;

  bits<5> RD;
  bits<5> RS1;
  bits<12> IMM;

  let Inst{31-20} = IMM{11-0};
  let Inst{19-15} = 0b00000;
  let Inst{14-12} = 0b010;
  let Inst{11 - 7} = RD;
  let Inst{6 - 0} = 0b0000100;
}

def READPM: InstIREDEFINE<"readpm", 0b0000000, 0b010       , add, GR32, GR32, imm32sx12>
{
 let Itinerary   = REDEFINESPM;
}
def DREADPM: InstIREDEFINE<"dreadpm", 0b0000001, 0b010       , add, GR32, GR32, imm32sx12>
{
 let Itinerary   = REDEFINESPM;
}

//REDEFINE S instructions

def END: InstREDEFINE<4, (outs), (ins imm32sx12:$src), 
                "end \t$src", 
                [],REDEFINEALU> {
  field bits<32> Inst;

  bits<5> RS1;
  bits<5> RS2;
  bits<12> IMM;

  let Inst{31-25} = IMM{11-5};
  let Inst{20-24} = 0b00000;
  let Inst{15-19} = 0b00000;
  let Inst{12-14} = 0b000;
  let Inst{7 - 11} = IMM{4-0};
  let Inst{6 - 0} = 0b0000000;
}

//def CREATEINST: InstIREDEFINE<"createinst", 0b0000100, 0b001, add, GR32, GR32, imm32sx12>
//{
// let Itinerary   = REDEFINEORC;
//}

def CREATEINST : InstREDEFINE<4, (outs GR32:$dst), (ins GR32:$src1, imm32sx12:$imm),
		"createinst\t$dst,\t$src1,\t$imm",
		[(set i32:$dst, (int_createinst i32:$src1, imm:$imm))], REDEFINEORC> {

  field bits<32> Inst;
  bits<5> RD;
  bits<5> RS1;
  bits<12> IMM;

  let Inst{31-20} = IMM{11-0};
  let Inst{15-19} = RS1;
  let Inst{12-14} = 0b001;
  let Inst{7 -11} = RD;
  let Inst{6 - 0} = 0b0000100;
}

//def WRITECM : InstSREDEFINE<"writecm" , 0b0100100, 0b000, add , GR32, GR32, imm32sx12>
//{
// let Itinerary   = REDEFINEORC;
//}

def MOVIMM : InstREDEFINE<4, (outs GR32:$dst), (ins imm32:$imm), "MOVIMM\t$dst,\t$imm", [], REDEFINEALU>{
	let isPseudo = 1;
}

def MOVADDR : InstREDEFINE<4, (outs GR32:$dst), (ins imm32:$imm), "MOVADDR\t$dst,\t$imm", [], REDEFINEALU>{
	let isPseudo = 1;
}

def WRITECM : InstREDEFINE<4, (outs), (ins GR32:$dst1, GR32:$src1, imm32sx12:$imm),
		"writecm\t$dst1,\t$src1,\t$imm",
		[(int_writecm i32:$dst1, i32:$src1, imm:$imm)], REDEFINEORC> {

  field bits<32> Inst;
  bits<5> RS2;
  bits<5> RS1;
  bits<12> IMM;

  let Inst{31-25} = IMM{11-5};
  let Inst{20-24} = RS2;
  let Inst{15-19} = RS1;
  let Inst{12-14} = 0b001;
  let Inst{7 -11} = IMM{4-0};
  let Inst{6 - 0} = 0b0000100;
}

//def WRITECMP : InstSREDEFINE<"writecmp" , 0b0100100, 0b010 , add , GR32, GR32, imm32sx12>
//{
// let Itinerary   = REDEFINEORC;
//}

def WRITECMP : InstREDEFINE<4, (outs), (ins GR32:$dst1, GR32:$src1, imm32sx12:$imm),
		"writecmp\t$dst1,\t$src1,\t$imm",
		[(int_writecmp i32:$dst1, i32:$src1, imm:$imm)], REDEFINEORC> {

  field bits<32> Inst;
  bits<5> RS2;
  bits<5> RS1;
  bits<12> IMM;

  let Inst{31-25} = IMM{11-5};
  let Inst{20-24} = RS2;
  let Inst{15-19} = RS1;
  let Inst{12-14} = 0b010;
  let Inst{7 -11} = IMM{4-0};
  let Inst{6 - 0} = 0b0000100;
}

def TRET : InstSREDEFINE<"tret" , 0b0100100, 0b001, add , GR32, GR32, imm32sx12>
{
 let Itinerary   = REDEFINEORC;
}

//def SYNC : InstSREDEFINE<"sync" , 0b0100100, 0b011, add , GR32, GR32, imm32sx12>
//{
// let Itinerary   = REDEFINEORC;
//}

def SYNC: InstREDEFINE<4, (outs), (ins GR32:$dst1, GR32:$src1, imm32sx12:$imm),
		"sync\t$dst1,\t$src1,\t$imm",
		[(int_sync i32:$dst1, i32:$src1, imm:$imm)], REDEFINEORC> {

  field bits<32> Inst;
  bits<5> RS2;
  bits<5> RS1;
  bits<12> IMM;

  let Inst{31-25} = IMM{11-5};
  let Inst{20-24} = RS2;
  let Inst{15-19} = RS1;
  let Inst{12-14} = 0b100;
  let Inst{7 -11} = IMM{4-0};
  let Inst{6 - 0} = 0b0000100;
}

//def FBIND : InstSREDEFINE<"fbind" , 0b0000100, 0b011, add , GR32, GR32, imm32sx12>
//{
// let Itinerary   = REDEFINEORC;
//}

def FBIND: InstREDEFINE<4, (outs), (ins GR32:$dst1, GR32:$src1, imm32sx12:$imm),
		"fbind\t$dst1,\t$src1,\t$imm",
		[(int_fbind i32:$dst1, i32:$src1, imm:$imm)], REDEFINEORC> {

  field bits<32> Inst;
  bits<5> RS2;
  bits<5> RS1;
  bits<12> IMM;

  let Inst{31-25} = IMM{11-5};
  let Inst{20-24} = RS2;
  let Inst{15-19} = RS1;
  let Inst{12-14} = 0b110;
  let Inst{7 -11} = IMM{4-0};
  let Inst{6 - 0} = 0b0000100;
}

def FDELETE: InstREDEFINE<4, (outs), (ins GR32:$id, imm32sx12:$imm), 
       "fdelete \t$id, $imm",
       [(int_fdelete i32:$id, imm:$imm)],REDEFINEORC> {

  field bits<32> Inst;
  bits<5> RS1;
  bits<12> IMM;

  let Inst{31-25} = IMM{11-5};
  let Inst{20-24} = 0b00000;
  let Inst{15-19} = RS1;
  let Inst{12-14} = 0b111;
  let Inst{7 - 11} = IMM{4-0};
  let Inst{6 - 0} = 0b0000000;
}

def WRITEPM : InstSREDEFINE<"writepm" , 0b0100000, 0b010, add , GR32, GR32, imm32sx12>
{
 let Itinerary   = REDEFINESPM;
}
/*
def DIV   : InstR<"div"  , 0b0110011, 0b0000001100, sdiv  , GR32, GR32>;
def DIVU  : InstR<"divu" , 0b0110011, 0b0000001101, udiv  , GR32, GR32>;
def REM   : InstR<"rem"  , 0b0110011, 0b0000001110, srem  , GR32, GR32>;
def REMU  : InstR<"remu" , 0b0110011, 0b0000001111, urem  , GR32, GR32>;
*/
def MUL   : InstR<"mul"  , 0b0110011, 0b0000001000, mul   , GR32, GR32>;
def MULH  : InstR<"mulh" , 0b0110011, 0b0000001001, mulhs , GR32, GR32>;
//TODO: no corresponding llvm ir instruction
//def MULHSU: InstR<"mulh", 0b0110011, 0b0000001010, mulhs , GR32, GR32>;
def MULHU : InstR<"mulhu", 0b0110011, 0b0000001011, mulhu , GR32, GR32>;

//Immediate values
def : Pat<(i32 imm32sx12:$imm), (MOVIMM imm32sx12:$imm)>; 
def : Pat<(i32 imm32:$imm), (MOVIMM imm32:$imm)>;
  
//FPU instructions
defm FADD_S : FPBinOps<"fadd.s", fadd, 0b00000, 0b00>;
defm FSUB_S : FPBinOps<"fsub.s", fsub, 0b00001, 0b00>;
defm FMUL_S : FPBinOps<"fmul.s", fmul, 0b00010, 0b00>;
defm FDIV_S : FPBinOps<"fdiv.s", fdiv, 0b00011, 0b00>;

def FMIN_S : InstR<"fmin.s", 0b1010011, 0b0010100000, fminnum, FP32, FP32>;
def FMAX_S : InstR<"fmax.s", 0b1010011, 0b0010100001, fmaxnum, FP32, FP32>;

//Single operand fp instructions
def FSQRT_S: InstFP<"fsqrt.s", 0b1010011, 0b000, 0b00000 ,0b0101100, fsqrt, FP32, FP32>;
def FSIN_S: InstFP<"fsin.s", 0b1111100, 0b000, 0b00000 ,0b0000000, fsin, FP32, FP32>;
def FCOS_S: InstFP<"fcos.s", 0b1111100, 0b001, 0b00000 ,0b0000000, fcos, FP32, FP32>;

defm FCVT_W_S  : FPConvOps<"fcvt.w.s",  null_frag, GR32, FP32, 0b01010, 0b00>;
defm FCVT_WU_S : FPConvOps<"fcvt.wu.s", null_frag, GR32, FP32, 0b01011, 0b00>;
defm FCVT_S_W  : FPConvOps<"fcvt.s.w",  sint_to_fp, FP32, GR32, 0b01110, 0b00>;
defm FCVT_S_WU : FPConvOps<"fcvt.s.wu", uint_to_fp, FP32, GR32, 0b01111, 0b00>;
//make sure we get the right rounding mode
def :Pat<(i32 (fp_to_uint FP32:$src)), (FCVT_WU_S_RTZ FP32:$src)>;
def :Pat<(i32 (fp_to_sint FP32:$src)), (FCVT_W_S_RTZ FP32:$src)>;

def FSGNJ_S : InstSign<"fsgnj.s", 0b1010011, 0b00101, 0b00, 0b000,
                        fcopysign, FP32, FP32>;
def FSGNJN_S : InstSign<"fsgnjn.s", 0b1010011, 0b00110, 0b00, 0b000,
                        fcopysign, FP32, FP32> {
                          let Pattern =
                          [(set FP32:$dst, (fcopysign FP32:$src1, (fneg FP32:$src2)))];
                        }
                        
//pattern is
//if signs are equal copysign from abs(src2)
//otherwise copysign from fabs( fneg (src2))
def FSGNJX_S : InstSign<"fsgnjx.s", 0b1010011, 0b00111, 0b00, 0b000,
    fcopysign, FP32, FP32> {
      let Pattern =
      [(set FP32:$dst, (select 
      (i32 (seteq (i32 (fgetsign FP32:$src1)), (i32 (fgetsign FP32:$src2)))),
        (fcopysign FP32:$src1, (fabs FP32:$src2)),
        (fcopysign FP32:$src1, (fneg (fabs FP32:$src2)))
      ))];
      }
      
//Move instruction (bitcasts)
def FMV_X_S : InstConv<"fmv.x.s", "", 0b1010011, 0b11100, 0b00, 0b000, bitconvert, GR32, FP32>;
def FMV_S_X : InstConv<"fmv.s.x", "", 0b1010011, 0b11110, 0b00, 0b000, bitconvert, FP32, GR32>;

//Floating point comparisons
def FEQ_S : InstSign<"feq.s", 0b1010011, 0b10101, 0b00, 0b000, setoeq, GR32, FP32>;
def FLT_S : InstSign<"flt.s", 0b1010011, 0b10110, 0b00, 0b000, setolt, GR32, FP32>;
def FLE_S : InstSign<"fle.s", 0b1010011, 0b10111, 0b00, 0b000, setole, GR32, FP32>;
def FUEQ_S : InstSign<"feq.s", 0b1010011, 0b10101, 0b00, 0b000, setueq, GR32, FP32>;
def FULT_S : InstSign<"flt.s", 0b1010011, 0b10110, 0b00, 0b000, setult, GR32, FP32>;
def FULE_S : InstSign<"fle.s", 0b1010011, 0b10111, 0b00, 0b000, setule, GR32, FP32>;
//synthesized set operators
multiclass FPCmpPats<RegisterOperand RC, Instruction FEQOp, Instruction FEQUOp,
                     Instruction FLTOp, Instruction FLTUOp,
                     Instruction FLEOp, Instruction FLEUOp> {
                //RISC-V operands go in reverse order
  //lhs > rhs ==> rhs < lhs
  def : Pat<(setogt RC:$lhs, RC:$rhs), (FLTOp RC:$lhs, RC:$rhs)>;
  def : Pat<(setugt RC:$lhs, RC:$rhs), (FLTUOp RC:$lhs, RC:$rhs)>;
  //lhs >= rhs ==> rhs =< lhs
  def : Pat<(setoge RC:$lhs, RC:$rhs), (FLEOp RC:$lhs, RC:$rhs)>;
  def : Pat<(setuge RC:$lhs, RC:$rhs), (FLEUOp RC:$lhs, RC:$rhs)>;
  //lhs != rhs ==> !(lhs == rhs) using seqz 
  def : Pat<(setone RC:$lhs, RC:$rhs), (SLTIU (FEQOp RC:$rhs, RC:$lhs), 1)>;
  def : Pat<(setune RC:$lhs, RC:$rhs), (SLTIU (FEQUOp RC:$rhs, RC:$lhs), 1)>;
}
defm : FPCmpPats<FP32, FEQ_S, FUEQ_S, FLT_S, FULT_S, FLE_S, FULE_S>;

//fp imm
def :Pat<(fpimm0), (FCVT_S_W_RDY zero)>;
  
              
//REDEFINE specific floating point instructions
def FWRITEPM : InstSREDEFINE<"fwritepm" , 0b0100000, 0b010, add , GR32, FP32,  imm32sx12>
{
 let Itinerary   = REDEFINESPM;
}
def FREADPM: InstIREDEFINE<"freadpm", 0b0000000, 0b010, add, FP32, GR32, imm32sx12>
{
 let Itinerary   = REDEFINESPM;
}
def FDREADPM: InstIREDEFINE<"fdreadpm", 0b0000000, 0b010, add, FP32, GR32, imm32sx12>
{
 let Itinerary   = REDEFINESPM;
}
  
//fmadd etc with rounding mode RNE
def FMADD:InstR4<"fmadd.s", 0b1000011, 0b000, fmul, fadd, FP32, FP32>;
def FMSUB:InstR4<"fmsub.s", 0b1000011, 0b000, fmul, fsub, FP32, FP32>;

def FNMSUB:InstREDEFINE<4, (outs FP32:$dst), (ins FP32:$src3, FP32:$src2, FP32:$src1), 
                "fnmsub.s $dst, $src1, $src2, $src3",[],REDEFINEFPA>{
  field bits<32> Inst;

  bits<5> RD;
  bits<5> RS1;
  bits<5> RS2;
  bits<5> RS3;

  let Inst{31-27} = RS3;
  let Inst{26-25} = 0b00;
  let Inst{24-20} = RS2;
  let Inst{19-15} = RS1;
  let Inst{14-12} = 000;
  let Inst{11-7} = RD;
  let Inst{6-0} = 0b1000011;
}

def FNMADD:InstREDEFINE<4, (outs FP32:$dst), (ins FP32:$src3, FP32:$src2, FP32:$src1), 
              	"fnmadd.s $dst, $src1, $src2, $src3", 
                [(set FP32:$dst, (fneg (fadd (fmul FP32:$src1, FP32:$src2), FP32:$src3)))],REDEFINEFPA>{
  field bits<32> Inst;

  bits<5> RD;
  bits<5> RS1;
  bits<5> RS2;
  bits<5> RS3;

  let Inst{31-27} = RS3;
  let Inst{26-25} = 0b00;
  let Inst{24-20} = RS2;
  let Inst{19-15} = RS1;
  let Inst{14-12} = 000;
  let Inst{11-7} = RD;
  let Inst{6-0} = 0b1000011;

}

def : Pat<(REDEFINEHi tglobaladdr:$in), (LUI tglobaladdr:$in)>;
def : Pat<(REDEFINELo tglobaladdr:$in), (ADDI zero, tglobaladdr:$in)>;


//Synthesized set operators
//patterns to be used for 32 and 64bit
multiclass SeteqPats<RegisterOperand RC, Instruction SLTiuOp, Instruction XOROp,
                     Instruction SLTuOp, Register ZEROReg> {
  def : Pat<(seteq RC:$lhs, RC:$rhs),
            (SLTiuOp (XOROp RC:$lhs, RC:$rhs), 1)>;
  def : Pat<(setne RC:$lhs, RC:$rhs),
            (SLTuOp (XOROp RC:$lhs, RC:$rhs), ZEROReg)>;
}

multiclass SetlePats<RegisterOperand RC, Instruction SLTOp, Instruction SLTuOp> {
  def : Pat<(setle RC:$lhs, RC:$rhs),
            (XORI (SLTOp RC:$lhs, RC:$rhs), 1)>;
  def : Pat<(setule RC:$lhs, RC:$rhs),
            (XORI (SLTuOp RC:$lhs, RC:$rhs), 1)>;
}

multiclass SetgtPats<RegisterOperand RC, Instruction SLTOp, Instruction SLTuOp> {
  def : Pat<(setgt RC:$lhs, RC:$rhs),
            (SLTOp RC:$lhs, RC:$rhs)>;
  def : Pat<(setugt RC:$lhs, RC:$rhs),
            (SLTuOp RC:$lhs, RC:$rhs)>;
}

multiclass SetgePats<RegisterOperand RC, Instruction SLTOp, Instruction SLTuOp> {
  def : Pat<(setge RC:$lhs, RC:$rhs),
            (XORI (SLTOp RC:$rhs, RC:$lhs), 1)>;
  def : Pat<(setuge RC:$lhs, RC:$rhs),
            (XORI (SLTuOp RC:$rhs, RC:$lhs), 1)>;
}

defm : SeteqPats<GR32, SLTIU, XOR, SLTU, zero>;
defm : SetlePats<GR32, SLT, SLTU>;
defm : SetgtPats<GR32, SLT, SLTU>;
defm : SetgePats<GR32, SLT, SLTU>;

//TODO: enforce constraints here or up on level?
def SLLI: InstI<"slli", 0b0010011, 0b001       , shl, GR32, GR32, imm32sx12>{
  let IMM{11-6} = 0b000000; 
  //trap if $imm{5}!=0 TODO:how to do this?
}
def SRLI: InstI<"srli", 0b0010011, 0b101       , srl, GR32, GR32, imm32sx12>{
  let IMM{11-6} = 0b000000; 
  //trap if $src{5}!=0 TODO:how to do this?
}
def SRAI: InstI<"srai", 0b0010011, 0b101       , sra, GR32, GR32, imm32sx12>{
  let IMM{11-6} = 0b000001; 
  //trap if $src{5}!=0 TODO:how to do this?
}

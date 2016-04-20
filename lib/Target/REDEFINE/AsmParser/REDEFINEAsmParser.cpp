//===-- REDEFINEAsmParser.cpp - Parse RISCV assembly instructions --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/REDEFINEMCTargetDesc.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCTargetAsmParser.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

// Return true if Expr is in the range [MinValue, MaxValue].
static bool inRange(const MCExpr *Expr, int64_t MinValue, int64_t MaxValue) {
  if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr)) {
    int64_t Value = CE->getValue();
    return Value >= MinValue && Value <= MaxValue;
  }
  return false;
}

namespace {
class REDEFINEOperand : public MCParsedAsmOperand {
public:
  enum RegisterKind {
    GR32Reg,
    FP32Reg
  };

private:
  enum OperandKind {
    KindToken,
    KindReg,
    KindImm,
    KindMem
  };

  OperandKind Kind;
  SMLoc StartLoc, EndLoc;

  // A string of length Length, starting at Data.
  struct TokenOp {
    const char *Data;
    unsigned Length;
  };

  // LLVM register Num, which has kind Kind.
  struct RegOp {
    RegisterKind Kind;
    unsigned Num;
  };

  // Base + Disp + Index, where Base and Index are LLVM registers or 0.
  // RegKind says what type the registers have
  struct MemOp {
    unsigned Base : 8;
    unsigned Index : 8;
    unsigned RegKind : 8;
    unsigned Unused : 8;
    const MCExpr *Disp;
  };

  union {
    TokenOp Token;
    RegOp Reg;
    const MCExpr *Imm;
    MemOp Mem;
  };

  REDEFINEOperand(OperandKind kind, SMLoc startLoc, SMLoc endLoc)
    : Kind(kind), StartLoc(startLoc), EndLoc(endLoc)
  {}

  void addExpr(MCInst &Inst, const MCExpr *Expr) const {
    // Add as immediates when possible.  Null MCExpr = 0.
    if (Expr == 0)
      Inst.addOperand(MCOperand::CreateImm(0));
    else if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr))
      Inst.addOperand(MCOperand::CreateImm(CE->getValue()));
    else
      Inst.addOperand(MCOperand::CreateExpr(Expr));
  }

public:
  // Create particular kinds of operand.
  static REDEFINEOperand *createToken(StringRef Str, SMLoc Loc) {
    REDEFINEOperand *Op = new REDEFINEOperand(KindToken, Loc, Loc);
    Op->Token.Data = Str.data();
    Op->Token.Length = Str.size();
    return Op;
  }
  static REDEFINEOperand *createReg(RegisterKind Kind, unsigned Num,
                                   SMLoc StartLoc, SMLoc EndLoc) {
    REDEFINEOperand *Op = new REDEFINEOperand(KindReg, StartLoc, EndLoc);
    Op->Reg.Kind = Kind;
    Op->Reg.Num = Num;
    return Op;
  }
  static REDEFINEOperand *createImm(const MCExpr *Expr, SMLoc StartLoc,
                                   SMLoc EndLoc) {
    REDEFINEOperand *Op = new REDEFINEOperand(KindImm, StartLoc, EndLoc);
    Op->Imm = Expr;
    return Op;
  }
  static REDEFINEOperand *createMem(RegisterKind RegKind, unsigned Base,
                                   const MCExpr *Disp, unsigned Index,
                                   SMLoc StartLoc, SMLoc EndLoc) {
    REDEFINEOperand *Op = new REDEFINEOperand(KindMem, StartLoc, EndLoc);
    Op->Mem.RegKind = RegKind;
    Op->Mem.Base = Base;
    Op->Mem.Index = Index;
    Op->Mem.Disp = Disp;
    return Op;
  }

  // Token operands
  virtual bool isToken() const LLVM_OVERRIDE {
    return Kind == KindToken;
  }
  StringRef getToken() const {
    assert(Kind == KindToken && "Not a token");
    return StringRef(Token.Data, Token.Length);
  }

  // Register operands.
  virtual bool isReg() const LLVM_OVERRIDE {
    return Kind == KindReg;
  }
  bool isReg(RegisterKind RegKind) const {
    return Kind == KindReg && Reg.Kind == RegKind;
  }
  virtual unsigned getReg() const LLVM_OVERRIDE {
    assert(Kind == KindReg && "Not a register");
    return Reg.Num;
  }

  // Immediate operands.
  virtual bool isImm() const LLVM_OVERRIDE {
    return Kind == KindImm;
  }
  bool isImm(int64_t MinValue, int64_t MaxValue) const {
    return Kind == KindImm && inRange(Imm, MinValue, MaxValue);
  }
  const MCExpr *getImm() const {
    assert(Kind == KindImm && "Not an immediate");
    return Imm;
  }

  // Memory operands.
  virtual bool isMem() const LLVM_OVERRIDE {
    return Kind == KindMem;
  }
  bool isMem(RegisterKind RegKind, bool HasIndex) const {
    return (Kind == KindMem &&
            Mem.RegKind == RegKind &&
            (HasIndex || !Mem.Index));
  }
  bool isMemDisp12(RegisterKind RegKind, bool HasIndex) const {
    return isMem(RegKind, HasIndex) && inRange(Mem.Disp, 0, 0xfff);
  }
  bool isMemDisp20(RegisterKind RegKind, bool HasIndex) const {
    return isMem(RegKind, HasIndex) && inRange(Mem.Disp, -524288, 524287);
  }

  // Override MCParsedAsmOperand.
  virtual SMLoc getStartLoc() const LLVM_OVERRIDE { return StartLoc; }
  virtual SMLoc getEndLoc() const LLVM_OVERRIDE { return EndLoc; }
  virtual void print(raw_ostream &OS) const LLVM_OVERRIDE;

  // Used by the TableGen code to add particular types of operand
  // to an instruction.
  void addRegOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands");
    Inst.addOperand(MCOperand::CreateReg(getReg()));
  }
  void addImmOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands");
    addExpr(Inst, getImm());
  }

  // Used by the TableGen code to check for particular operand types.
  bool isGR32() const { return isReg(GR32Reg); }
  bool isFP32() const { return isReg(FP32Reg); }
  bool isU4Imm() const { return isImm(0, 15); }
  bool isU12Imm() const { return isImm(0, 4096); }
  bool isS12Imm() const { return isImm(-2048, 2047); }
  bool isU20Imm() const { return isImm(0, 1048576); }
  bool isS20Imm() const { return isImm(-524288, 524287); }
  bool isU32Imm() const { return isImm(0, (1LL << 32) - 1); }
  bool isS32Imm() const { return isImm(-(1LL << 31), (1LL << 31) - 1); }
  bool isU64Imm() const { return isImm(0, 18446744073709551615UL); }
  bool isS64Imm() const { return isImm(-9223372036854775807LL,9223372036854775807LL); }
};

// Maps of asm register numbers to LLVM register numbers, with 0 indicating
// an invalid register.  We don't use register class directly because that
// specifies the allocation order.
static const unsigned GR32Regs[] = {
  REDEFINE::zero, REDEFINE::ra, REDEFINE::sp, REDEFINE::gp, REDEFINE::tp,
  REDEFINE::t0, REDEFINE::t1, REDEFINE::t2,
  REDEFINE::s0, REDEFINE::s1,
  REDEFINE::a0, REDEFINE::a1, REDEFINE::a2, REDEFINE::a3, REDEFINE::a4, REDEFINE::a5, REDEFINE::a6, REDEFINE::a7, 
  REDEFINE::s2, REDEFINE::s3, REDEFINE::s4, REDEFINE::s5, REDEFINE::s6, REDEFINE::s7, REDEFINE::s8, REDEFINE::s9, REDEFINE::s10, REDEFINE::s11,
  REDEFINE::t3, REDEFINE::t4, REDEFINE::t5
};

static const unsigned FP32Regs[] = {
  REDEFINE::ft0, REDEFINE::ft1, REDEFINE::ft2, REDEFINE::ft3, REDEFINE::ft4, REDEFINE::ft5, REDEFINE::ft6, REDEFINE::ft7,
  REDEFINE::fs0, REDEFINE::fs1,
  REDEFINE::fa0, REDEFINE::fa1, REDEFINE::fa2, REDEFINE::fa3, REDEFINE::fa4, REDEFINE::fa5, REDEFINE::fa6, REDEFINE::fa7,
  REDEFINE::fs2, REDEFINE::fs3, REDEFINE::fs4, REDEFINE::fs5, REDEFINE::fs6, REDEFINE::fs7, REDEFINE::fs8, REDEFINE::fs9, REDEFINE::fs10, REDEFINE::fs11,
  REDEFINE::ft8, REDEFINE::ft9, REDEFINE::ft10, REDEFINE::ft11
};

class REDEFINEAsmParser : public MCTargetAsmParser {
#define GET_ASSEMBLER_HEADER
#include "REDEFINEGenAsmMatcher.inc"

private:
  MCSubtargetInfo &STI;
  MCAsmParser &Parser;
  struct Register {
    char Prefix;
    unsigned Number;
    SMLoc StartLoc, EndLoc;
  };

  bool parseRegister(Register &Reg);

  OperandMatchResultTy
  parseRegister(Register &Reg, char Prefix, const unsigned *Regs,
                bool IsAddress = false);

  OperandMatchResultTy
  parseRegister(SmallVectorImpl<MCParsedAsmOperand*> &Operands,
                char Prefix, const unsigned *Regs,
                REDEFINEOperand::RegisterKind Kind,
                bool IsAddress = false);

  OperandMatchResultTy
  parseAddress(SmallVectorImpl<MCParsedAsmOperand*> &Operands,
               const unsigned *Regs, REDEFINEOperand::RegisterKind RegKind,
               bool HasIndex);

  bool parseOperand(SmallVectorImpl<MCParsedAsmOperand*> &Operands,
                    StringRef Mnemonic);

public:
  REDEFINEAsmParser(MCSubtargetInfo &sti, MCAsmParser &parser)
    : MCTargetAsmParser(), STI(sti), Parser(parser) {
    MCAsmParserExtension::Initialize(Parser);

    // Initialize the set of available features.
    setAvailableFeatures(ComputeAvailableFeatures(STI.getFeatureBits()));
  }

  // Override MCTargetAsmParser.
  virtual bool ParseDirective(AsmToken DirectiveID) LLVM_OVERRIDE;
  virtual bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc,
                             SMLoc &EndLoc) LLVM_OVERRIDE;
  virtual bool ParseInstruction(ParseInstructionInfo &Info,
                                StringRef Name, SMLoc NameLoc,
                                SmallVectorImpl<MCParsedAsmOperand*> &Operands)
    LLVM_OVERRIDE;
  virtual bool
    MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                            SmallVectorImpl<MCParsedAsmOperand*> &Operands,
                            MCStreamer &Out, unsigned &ErrorInfo,
                            bool MatchingInlineAsm) LLVM_OVERRIDE;

  // Used by the TableGen code to parse particular operand types.
  OperandMatchResultTy
  parseGR32(SmallVectorImpl<MCParsedAsmOperand*> &Operands) {
    return parseRegister(Operands, 'x', GR32Regs, REDEFINEOperand::GR32Reg);
  }

  // Used by the TableGen code to parse particular operand types.
  OperandMatchResultTy
  parseFP32(SmallVectorImpl<MCParsedAsmOperand*> &Operands) {
    return parseRegister(Operands, 'f', FP32Regs, REDEFINEOperand::FP32Reg);
  }

};
}

#define GET_REGISTER_MATCHER
#define GET_SUBTARGET_FEATURE_NAME
#define GET_MATCHER_IMPLEMENTATION
#include "REDEFINEGenAsmMatcher.inc"

void REDEFINEOperand::print(raw_ostream &OS) const {
  llvm_unreachable("Not implemented");
}

// Parse one register of the form %<prefix><number>.
bool REDEFINEAsmParser::parseRegister(Register &Reg) {
  Reg.StartLoc = Parser.getTok().getLoc();

  // Expect a register name.
  if (Parser.getTok().isNot(AsmToken::Identifier))
    return true;

  // Check the prefix.
  StringRef Name = Parser.getTok().getString();
  if (Name.size() < 2)
    return true;
  Reg.Prefix = Name[0];

  // Treat the rest of the register name as a register number.
  if (Name.substr(1).getAsInteger(10, Reg.Number))
    return true;

  Reg.EndLoc = Parser.getTok().getLoc();
  Parser.Lex();
  return false;
}

// Parse a register with prefix Prefix and convert it to LLVM numbering.
// Regs maps asm register numbers to LLVM register numbers, with zero
// entries indicating an invalid register.  IsAddress says whether the
// register appears in an address context.
REDEFINEAsmParser::OperandMatchResultTy
REDEFINEAsmParser::parseRegister(Register &Reg, char Prefix,
                                const unsigned *Regs, bool IsAddress) {
  if (parseRegister(Reg))
    return MatchOperand_NoMatch;
  if (Reg.Prefix != Prefix || Reg.Number > 15 || Regs[Reg.Number] == 0) {
    Error(Reg.StartLoc, "invalid register");
    return MatchOperand_ParseFail;
  }
  if (Reg.Number == 0 && IsAddress) {
    Error(Reg.StartLoc, "%r0 used in an address");
    return MatchOperand_ParseFail;
  }
  Reg.Number = Regs[Reg.Number];
  return MatchOperand_Success;
}

// Parse a register and add it to Operands.  Prefix is 'r' for GPRs,
// 'f' for FPRs, etc.  Regs maps asm register numbers to LLVM register numbers,
// with zero entries indicating an invalid register.  Kind is the type of
// register represented by Regs and IsAddress says whether the register is
// being parsed in an address context, meaning that %r0 evaluates as 0.
REDEFINEAsmParser::OperandMatchResultTy
REDEFINEAsmParser::parseRegister(SmallVectorImpl<MCParsedAsmOperand*> &Operands,
                                char Prefix, const unsigned *Regs,
                                REDEFINEOperand::RegisterKind Kind,
                                bool IsAddress) {
  Register Reg;
  OperandMatchResultTy Result = parseRegister(Reg, Prefix, Regs, IsAddress);
  if (Result == MatchOperand_Success)
    Operands.push_back(REDEFINEOperand::createReg(Kind, Reg.Number,
                                                 Reg.StartLoc, Reg.EndLoc));
  return Result;
}

// Parse a memory operand and add it to Operands.  Regs maps asm register
// numbers to LLVM address registers and RegKind says what kind of address
// register we're using (GR32Reg or GR64Reg).  HasIndex says whether
// the address allows index registers.
REDEFINEAsmParser::OperandMatchResultTy
REDEFINEAsmParser::parseAddress(SmallVectorImpl<MCParsedAsmOperand*> &Operands,
                               const unsigned *Regs,
                               REDEFINEOperand::RegisterKind RegKind,
                               bool HasIndex) {
  SMLoc StartLoc = Parser.getTok().getLoc();

  // Parse the displacement, which must always be present.
  const MCExpr *Disp;
  if (getParser().parseExpression(Disp))
    return MatchOperand_NoMatch;

  // Parse the optional base and index.
  unsigned Index = 0;
  unsigned Base = 0;
  if (getLexer().is(AsmToken::LParen)) {
    Parser.Lex();

    // Parse the first register.
    Register Reg;
    OperandMatchResultTy Result = parseRegister(Reg, 'x', GR32Regs, true);
    if (Result != MatchOperand_Success)
      return Result;

    // Check whether there's a second register.  If so, the one that we
    // just parsed was the index.
    if (getLexer().is(AsmToken::Comma)) {
      Parser.Lex();

      if (!HasIndex) {
        Error(Reg.StartLoc, "invalid use of indexed addressing");
        return MatchOperand_ParseFail;
      }

      Index = Reg.Number;
      Result = parseRegister(Reg, 'x', GR32Regs, true);
      if (Result != MatchOperand_Success)
        return Result;
    }
    Base = Reg.Number;

    // Consume the closing bracket.
    if (getLexer().isNot(AsmToken::RParen))
      return MatchOperand_NoMatch;
    Parser.Lex();
  }

  SMLoc EndLoc =
    SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
  Operands.push_back(REDEFINEOperand::createMem(RegKind, Base, Disp, Index,
                                               StartLoc, EndLoc));
  return MatchOperand_Success;
}

bool REDEFINEAsmParser::ParseDirective(AsmToken DirectiveID) {
  return true;
}

bool REDEFINEAsmParser::ParseRegister(unsigned &RegNo, SMLoc &StartLoc,
                                     SMLoc &EndLoc) {
  Register Reg;
  if (parseRegister(Reg))
    return Error(Reg.StartLoc, "register expected");
  if (Reg.Prefix == 'x' && Reg.Number < 32)
    RegNo = GR32Regs[Reg.Number];
  else
    return Error(Reg.StartLoc, "invalid register");
  StartLoc = Reg.StartLoc;
  EndLoc = Reg.EndLoc;
  return false;
}

bool REDEFINEAsmParser::
ParseInstruction(ParseInstructionInfo &Info, StringRef Name, SMLoc NameLoc,
                 SmallVectorImpl<MCParsedAsmOperand*> &Operands) {
  Operands.push_back(REDEFINEOperand::createToken(Name, NameLoc));

  // Read the remaining operands.
  if (getLexer().isNot(AsmToken::EndOfStatement)) {
    // Read the first operand.
    if (parseOperand(Operands, Name)) {
      Parser.eatToEndOfStatement();
      return true;
    }

    // Read any subsequent operands.
    while (getLexer().is(AsmToken::Comma)) {
      Parser.Lex();
      if (parseOperand(Operands, Name)) {
        Parser.eatToEndOfStatement();
        return true;
      }
    }
    if (getLexer().isNot(AsmToken::EndOfStatement)) {
      SMLoc Loc = getLexer().getLoc();
      Parser.eatToEndOfStatement();
      return Error(Loc, "unexpected token in argument list");
    }
  }

  // Consume the EndOfStatement.
  Parser.Lex();
  return false;
}

bool REDEFINEAsmParser::
parseOperand(SmallVectorImpl<MCParsedAsmOperand*> &Operands,
             StringRef Mnemonic) {
  // Check if the current operand has a custom associated parser, if so, try to
  // custom parse the operand, or fallback to the general approach.
  OperandMatchResultTy ResTy = MatchOperandParserImpl(Operands, Mnemonic);
  if (ResTy == MatchOperand_Success)
    return false;

  // If there wasn't a custom match, try the generic matcher below. Otherwise,
  // there was a match, but an error occurred, in which case, just return that
  // the operand parsing failed.
  if (ResTy == MatchOperand_ParseFail)
    return true;

  // The only other type of operand is an immediate.
  const MCExpr *Expr;
  SMLoc StartLoc = Parser.getTok().getLoc();
  if (getParser().parseExpression(Expr))
    return true;

  SMLoc EndLoc =
    SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
  Operands.push_back(REDEFINEOperand::createImm(Expr, StartLoc, EndLoc));
  return false;
}

bool REDEFINEAsmParser::
MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                        SmallVectorImpl<MCParsedAsmOperand*> &Operands,
                        MCStreamer &Out, unsigned &ErrorInfo,
                        bool MatchingInlineAsm) {
  MCInst Inst;
  unsigned MatchResult;

  MatchResult = MatchInstructionImpl(Operands, Inst, ErrorInfo,
                                     MatchingInlineAsm);
  switch (MatchResult) {
  default: break;
  case Match_Success:
    Inst.setLoc(IDLoc);
    Out.EmitInstruction(Inst);
    return false;

  case Match_MissingFeature: {
    assert(ErrorInfo && "Unknown missing feature!");
    // Special case the error message for the very common case where only
    // a single subtarget feature is missing
    std::string Msg = "instruction requires:";
    unsigned Mask = 1;
    for (unsigned I = 0; I < sizeof(ErrorInfo) * 8 - 1; ++I) {
      if (ErrorInfo & Mask) {
        Msg += " ";
        Msg += getSubtargetFeatureName(ErrorInfo & Mask);
      }
      Mask <<= 1;
    }
    return Error(IDLoc, Msg);
  }

  case Match_InvalidOperand: {
    SMLoc ErrorLoc = IDLoc;
    if (ErrorInfo != ~0U) {
      if (ErrorInfo >= Operands.size())
        return Error(IDLoc, "too few operands for instruction");

      ErrorLoc = ((REDEFINEOperand*)Operands[ErrorInfo])->getStartLoc();
      if (ErrorLoc == SMLoc())
        ErrorLoc = IDLoc;
    }
    return Error(ErrorLoc, "invalid operand for instruction");
  }

  case Match_MnemonicFail:
    return Error(IDLoc, "invalid instruction");
  }

  llvm_unreachable("Unexpected match type");
}

// Force static initialization.
extern "C" void LLVMInitializeREDEFINEAsmParser() {
  RegisterMCAsmParser<REDEFINEAsmParser> X(TheREDEFINETarget);
}

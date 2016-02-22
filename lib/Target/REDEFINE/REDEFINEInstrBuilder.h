//===-- REDEFINEInstrBuilder.h - Functions to aid building insts -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file exposes functions that may be used with BuildMI from the
// MachineInstrBuilder.h file to handle REDEFINE'isms in a clean way.
//
//===----------------------------------------------------------------------===//

#ifndef REDEFINEINSTRBUILDER_H
#define REDEFINEINSTRBUILDER_H

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/PseudoSourceValue.h"

namespace llvm {
//TODO add context frame stuff here
} // End llvm namespace

#endif

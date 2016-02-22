//===-- REDEFINEOperators.td - REDEFINE-specific operators ------*- tblgen-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Node definitions
//===----------------------------------------------------------------------===//

//global addr
def REDEFINEHi    : SDNode<"REDEFINEISD::Hi", SDTIntUnaryOp>;
def REDEFINELo    : SDNode<"REDEFINEISD::Lo", SDTIntUnaryOp>;

//===----------------------------------------------------------------------===//
// Pattern fragments
//===----------------------------------------------------------------------===//

// Register sign-extend operations.  Sub-32-bit values are represented as i32s.
def sext8  : PatFrag<(ops node:$src), (sext_inreg node:$src, i8)>;
def sext16 : PatFrag<(ops node:$src), (sext_inreg node:$src, i16)>;
//def sext32 : PatFrag<(ops node:$src), (sext (i32 node:$src))>;

// Register zero-extend operations.  Sub-32-bit values are represented as i32s.
def zext8  : PatFrag<(ops node:$src), (and node:$src, 0xff)>;
def zext16 : PatFrag<(ops node:$src), (and node:$src, 0xffff)>;
//def zext32 : PatFrag<(ops node:$src), (zext (i32 node:$src))>;

// Aligned loads.
class AlignedLoad<SDPatternOperator load>
  : PatFrag<(ops node:$addr), (load node:$addr), [{
  LoadSDNode *Load = cast<LoadSDNode>(N);
  return Load->getAlignment() >= Load->getMemoryVT().getStoreSize();
}]>;
def aligned_load        : AlignedLoad<load>;
def aligned_sextloadi16 : AlignedLoad<sextloadi16>;
def aligned_sextloadi32 : AlignedLoad<sextloadi32>;
def aligned_zextloadi16 : AlignedLoad<zextloadi16>;
def aligned_zextloadi32 : AlignedLoad<zextloadi32>;

// Aligned stores.
class AlignedStore<SDPatternOperator store>
  : PatFrag<(ops node:$src, node:$addr), (store node:$src, node:$addr), [{
  StoreSDNode *Store = cast<StoreSDNode>(N);
  return Store->getAlignment() >= Store->getMemoryVT().getStoreSize();
}]>;
def aligned_store         : AlignedStore<store>;
def aligned_truncstorei16 : AlignedStore<truncstorei16>;
def aligned_truncstorei32 : AlignedStore<truncstorei32>;

// Create a unary operator that loads from memory and then performs
// the given operation on it.
class loadu<SDPatternOperator operator>
  : PatFrag<(ops node:$addr), (operator (load node:$addr))>;

// Create a store operator that performs the given unary operation
// on the value before storing it.
class storeu<SDPatternOperator operator>
  : PatFrag<(ops node:$value, node:$addr),
            (store (operator node:$value), node:$addr)>;

def SDT_RWrapPtr            : SDTypeProfile<1, 1,
                                            [SDTCisSameAs<0, 1>,
                                             SDTCisPtrTy<0>]>;
def r_pcrel_wrapper     : SDNode<"REDEFINEISD::PCREL_WRAPPER", SDT_RWrapPtr, []>;
        
// Typed floating-point loads.
def loadf32 : PatFrag<(ops node:$src), (f32 (load node:$src))>;
def loadf64 : PatFrag<(ops node:$src), (f64 (load node:$src))>;
/*
 * Lattice.cpp
 *
 *  Created on: 03-Mar-2013
 *      Author: kavitha
 */

#include "Lattice.h"
using namespace llvm;

Lattice::Lattice(Value * instructionValue, LatticeValue latticeValue) {
	this->instructionValue = instructionValue;
	this->latticeValue = latticeValue;
}

Lattice::~Lattice() {
}

LatticeValue Lattice::getLatticeValue(){
	return this->latticeValue;
}
Value * Lattice::getInstructionValue(){
	return this->instructionValue;
}

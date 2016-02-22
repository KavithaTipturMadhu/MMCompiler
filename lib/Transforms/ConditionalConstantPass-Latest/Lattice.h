/*
 * Lattice.h
 *
 *  Created on: 03-Mar-2013
 *      Author: kavitha
 */

#ifndef LATTICE_H_
#define LATTICE_H_
#include "llvm/IR/Value.h"
namespace llvm {
enum LatticeValue {
	//Not A constant: could be a variable or phi function with one constant and one NAC
	NAC,

	CONSTANT,

	UNDEFINED
};

class Lattice {
private:
	LatticeValue latticeValue;
	Value * instructionValue;
public:
	Lattice(Value * instructionValue, LatticeValue latticeValue);
	virtual ~Lattice();
	LatticeValue getLatticeValue();
	Value * getInstructionValue();
};
}

#endif /* LATTICE_H_ */

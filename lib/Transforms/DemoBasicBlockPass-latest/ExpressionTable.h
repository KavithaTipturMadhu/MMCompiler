/*
 * ExpressionTable.h
 *
 *  Created on: 19-Feb-2013
 *      Author: kavitha
 */

#ifndef EXPRESSIONTABLE_H_
#define EXPRESSIONTABLE_H_
#include "ValueNumberTable.h"
#include "llvm/IR/Constants.h"
using namespace llvm;
#include <list>
using namespace std;

namespace llvm {
class ExpressionTableEntry {
public:
	Instruction * key;
	vector<int> valueNumber;
	ExpressionTableEntry(Instruction * key, int value) {
		this->key = key;
		valueNumber.push_back(value);
	}
};
class ExpressionTable {
public:
	ExpressionTable();

	~ExpressionTable();

	void insert(Instruction * key, int valueNumber);

	//Remove a key entry from the table
	void remove(Instruction * key);

	// Return the value associated with the given key in hash table.
	// Return empty string if key not found.
	int find(Instruction * key);

	bool compareInstructions(Instruction *source,Instruction *target);

	void clear();

	void removeInstructionsUsingOperand(Value * operand);

private:
	vector<ExpressionTableEntry> table;
};
}

#endif /* EXPRESSIONTABLE_H_ */

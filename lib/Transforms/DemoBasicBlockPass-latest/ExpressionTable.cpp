/*
 * ExpressionTable.cpp
 *
 *  Created on: 19-Feb-2013
 *      Author: kavitha
 */
#include "ExpressionTable.h"

ExpressionTable::ExpressionTable() {
}

ExpressionTable::~ExpressionTable() {
}

void ExpressionTable::insert(Instruction * key, int valueNumber) {
	vector<ExpressionTableEntry>::iterator itr;
	for (itr = table.begin(); itr < table.end(); itr++) {
		if (compareInstructions(itr->key, key)) {
			itr->valueNumber.push_back(valueNumber);
		}
	}

	ExpressionTableEntry newEntry(key, valueNumber);
	this->table.push_back(newEntry);
}

//Remove a key entry from the table
void ExpressionTable::remove(Instruction * key) {
	vector<ExpressionTableEntry>::iterator itr;
	int i = 0;
	for (itr = table.begin(); itr < table.end(); itr++) {
		if (compareInstructions(itr->key, key)) {
			table.erase(itr);
			return;
		}
	}
}

// Return the value associated with the given key in hash table.
// Return empty string if key not found.
int ExpressionTable::find(Instruction * key) {
	vector<ExpressionTableEntry>::iterator itr;
	for (itr = table.begin(); itr < table.end(); itr++) {
		if (compareInstructions(itr->key, key)) {
			vector<int>::iterator valItr;
			valItr = (itr->valueNumber.end());
			valItr--;
			return *valItr;
		}
	}
	return -1;
}

bool ExpressionTable::compareInstructions(Instruction *source, Instruction *target) {
	if (source->getOpcode() != target->getOpcode()) {
		return false;
	}
	int i;
	for (i = 0; i < source->getNumOperands(); i++) {
		Value * firstInstrOp = source->getOperand(i);
		Value * secondInstrOp = target->getOperand(i);
		if ((isa<Constant>(firstInstrOp) && !isa<Constant>(secondInstrOp)) || (isa<Instruction>(firstInstrOp) && !isa<Instruction>(secondInstrOp))) {
			return false;
		}
		if (isa<Constant>(firstInstrOp) && !ConstantExpr::getSub((Constant*) firstInstrOp, (Constant*) secondInstrOp)->isZeroValue()) {
			return false;
		} else if (isa<Instruction>(firstInstrOp) && ((Instruction*) firstInstrOp)->getOperand(0)->getName() != ((Instruction*) secondInstrOp)->getOperand(0)->getName()) {
			return false;
		}
	}
	return true;
}

void ExpressionTable::removeInstructionsUsingOperand(Value * operand) {
	vector<ExpressionTableEntry>::iterator itr;
	list<Instruction *> instructionList;
	for (itr = table.begin(); itr < table.end(); itr++) {
		Instruction *instruction = itr->key;
		for (int i = 0; i < instruction->getNumOperands(); i++) {
			Value *instructionOperand = instruction->getOperand(i);
			if (!isa<Constant>(instructionOperand) && operand == instructionOperand) {
				instructionList.push_back(instruction);
			}
		}
	}

	//Remove unnecessary instructions from the basic block
	for (list<Instruction*>::const_iterator it = instructionList.begin(); it != instructionList.end(); it++) {
		remove(*it);
	}
}

void ExpressionTable::clear() {
	table.clear();
}

/*
 * ValueNumberTable.h
 *
 *  Created on: 19-Feb-2013
 *      Author: kavitha
 */

#ifndef VALUENUMBERTABLE_H_
#define VALUENUMBERTABLE_H_
#define NUM_BUCKETS 1000
#include <vector>
#include <string>
#include <iostream>
using namespace std;
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Constant.h"
using namespace llvm;

namespace llvm {
class ValueNumberTableEntry {
public:
	Value * key;
	vector<int> valueNumber;
	ValueNumberTableEntry(Value * key, int value) {
		this->key = key;
		valueNumber.push_back(value);
	}
};

class ValueNumberTable {
public:
	ValueNumberTable();

	// Add a key-value pair to hash table
	int insert(Value * key);

	//Remove a key entry from the table
	void remove(Value * key);

	// Return the value associated with the given key in hash table.
	// Return empty string if key not found.
	int find(Value * key);

	void clear();

	Value * findInstructionWithValueNumber(int valueNumber);

protected:
	vector<ValueNumberTableEntry> table;
	int valueNumberCounter;
};
}
#endif /* VALUENUMBERTABLE_H_ */

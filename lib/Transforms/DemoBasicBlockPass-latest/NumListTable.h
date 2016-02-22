/*
 * NumberTable.h
 *
 *  Created on: 19-Feb-2013
 *      Author: kavitha
 */

#ifndef NUMBERTABLE_H_
#define NUMBERTABLE_H_
#include <vector>
using namespace std;

#include "llvm/IR/Constant.h"
using namespace llvm;

namespace llvm {
class NumTableEntry {
public:
	vector<Value *> numList;
	Constant *constantValue;
	bool constFlag;
	int valueNumber;

	NumTableEntry(Value * key, Constant *constantValue, bool isConstant, int valueNumber) {
		this->numList.push_back(key);
		this->constantValue = constantValue;
		this->constFlag = isConstant;
		this->valueNumber = valueNumber;
	}
};

class NumListTable {
private:
	vector<NumTableEntry> numTable;
public:
	NumListTable();
	virtual ~NumListTable();
	Constant * getConstantValue(int valueNumber);
	void addNewNumberInNumberList(int valueNumber, Value * number, Constant*constant);
	void setConstantFlag(int valueNumber, bool flag);
	void clear();
};
}
#endif /* NUMBERTABLE_H_ */

/*
 * NumberTable.cpp
 *
 *  Created on: 19-Feb-2013
 *      Author: kavitha
 */

#include "NumListTable.h"


NumListTable::NumListTable() {
}

NumListTable::~NumListTable() {
}

void NumListTable::setConstantFlag(int valueNumber, bool flag) {
	vector<NumTableEntry>::iterator itr;
	for (itr = this->numTable.begin(); itr < this->numTable.end(); itr++) {
		if (itr->valueNumber == valueNumber) {
			itr->constFlag = false;
		}
	}
}

void NumListTable::clear() {
	numTable.clear();
}

Constant * NumListTable::getConstantValue(int valueNumber) {
	vector<NumTableEntry>::iterator itr;
	for (itr = this->numTable.begin(); itr < this->numTable.end(); itr++) {
		if (itr->constFlag && itr->valueNumber == valueNumber) {
			return itr->constantValue;
		}
	}
	return 0;
}

void NumListTable::addNewNumberInNumberList(int valueNumber, Value * number, Constant * constantValue) {
	vector<NumTableEntry>::iterator itr;
	bool valNumAdded = false;
	for (itr = this->numTable.begin(); itr < this->numTable.end(); itr++) {
		if (itr->constFlag && itr->valueNumber == valueNumber) {
			itr->numList.push_back(number);
			valNumAdded = true;
			break;
		}
	}

	if (!valNumAdded) {
		NumTableEntry newNumber(number, constantValue, true, valueNumber);
		numTable.push_back(newNumber);
	}
}

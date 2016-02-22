/*
 * ValueNumberTable.cpp
 *
 *  Created on: 19-Feb-2013
 *      Author: kavitha
 */

#include "ValueNumberTable.h"

ValueNumberTable::ValueNumberTable() {
	valueNumberCounter = 0;
}

void ValueNumberTable::clear() {
	table.clear();
	valueNumberCounter = 0;
}

Value * ValueNumberTable::findInstructionWithValueNumber(int valueNumber) {
	vector<ValueNumberTableEntry>::iterator itr;
	for (itr = table.begin(); itr < table.end(); itr++) {
		vector<int>::iterator valItr;
		for (valItr = itr->valueNumber.begin(); valItr < itr->valueNumber.end(); valItr++) {
			if(valueNumber==*valItr){
				return itr->key;
			}
		}
	}
	return NULL;
}


int ValueNumberTable::insert(Value * key) {
	vector<ValueNumberTableEntry>::iterator itr;
	for (itr = table.begin(); itr < table.end(); itr++) {
		if (itr->key == key) {
			itr->valueNumber.push_back(valueNumberCounter);
			return valueNumberCounter++;
		}
	}
	ValueNumberTableEntry newEntry(key, valueNumberCounter);
	this->table.push_back(newEntry);
	return valueNumberCounter++;
}

int ValueNumberTable::find(Value * key) {
	vector<ValueNumberTableEntry>::iterator itr;
	for (itr = table.begin(); itr < table.end(); itr++) {
		if (itr->key == key) {
			vector<int>::iterator valItr;
			valItr = (itr->valueNumber.end());
			valItr--;
			return *valItr;
		}
	}
	return -1;
}

void ValueNumberTable::remove(Value * key) {
	vector<ValueNumberTableEntry>::iterator itr;
	int i = 0;
	for (itr = table.begin(); itr < table.end(); itr++) {
		if (itr->key == key) {
			table.erase(itr);
			return;
		}
	}
}

/*
 * InterHyperOpData.h
 *
 *  Created on: 09-Jul-2015
 *      Author: kavitha
 */

#ifndef INCLUDE_LLVM_IR_INTERHYPEROPDATA_H_
#define INCLUDE_LLVM_IR_INTERHYPEROPDATA_H_
#include <list>
#include <string>
#include <iterator>
using namespace std;

/**
 * Supported aggregate types:
 * 0. SCALAR: single value
 * 1. RANGE: Range of integers/char only. Can be a union of ranges
 * 2. REGION: region{lb1,lb2...lbn,ub1,ub2...ubn}:{var1,var2....varn}:{lb1<=var1, var1<=ub1,...}. Region is instantiated using values for lb1,ub1,lb2,ub2....
 * 			  An inner variable (Eg:var2 is inner to var1) can be a function of the outer variables i.e., f1(var1)<=var2<=f2(var1). This is for supporting non-rectangular polyhedra
 * 3. UNION: Union of any of the above aggregate types. Eg: {RANGE1},{RANGE2}
 */

class AggregateData {
public:
	list<unsigned int> volume;
	virtual list<unsigned int> getVolume(){
		return volume;
	}
};

class Scalar: public AggregateData {
public:
	Scalar() :
			AggregateData() {
		volume.push_back(1);
	}
};

class Range: public AggregateData {
public:
	double Start;
	double End;
	Range(double Start, double End) :
			AggregateData() {
		this->Start = Start;
		this->End = End;
		volume.push_back(0);
		volume.push_back(1);
	}

};
class Region: public AggregateData {
public:
	list<string> ProgramVariables;
	list<string> IterationVariables;
	list<string> Inequalities;
	Region(list<string> Inequalities) :
			AggregateData() {
		this->Inequalities = Inequalities;
		//	TODO
		for (int i = 0; i < this->Inequalities.size() - 1; i++) {
			volume.push_back(0);
		}
		volume.push_back(1);
	}
};

class Union: public AggregateData {
public:
	int count;
	list<AggregateData*> ContainedAggregates;

	Union(int count, list<AggregateData*> ContainedAggregates) :
			AggregateData() {
		this->count = count;
		std::copy(ContainedAggregates.begin(), ContainedAggregates.end(), std::back_inserter(this->ContainedAggregates));
	}
	list<unsigned int> getVolume() {
		//		TODO
		list<unsigned int> maxVolumeOrder;
		bool firstIteration = true;
		for (list<AggregateData *>::iterator itr = ContainedAggregates.begin(); itr != ContainedAggregates.end(); itr++) {
			if (firstIteration || maxVolumeOrder > (*itr)->getVolume()) {
				maxVolumeOrder = (*itr)->getVolume();
				firstIteration = false;
			}
		}
		return maxVolumeOrder;
	}
};

#endif /* INCLUDE_LLVM_IR_INTERHYPEROPDATA_H_ */

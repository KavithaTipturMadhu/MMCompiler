/*
 * HyperOpMetadataParser.h
 *
 *  Created on: 06-Jul-2015
 *      Author: kavitha
 */

#ifndef LIB_TARGET_RISCV_HYPEROPMETADATAPARSER_H_
#define LIB_TARGET_RISCV_HYPEROPMETADATAPARSER_H_

#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
using namespace llvm;
#include <map>
#include <list>
using namespace std;

#include "llvm/IR/InterHyperOpData.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "REDEFINEUtils.h"
#include "HyperOpInteractionGraph.h"

//const string TASKS_LIST = "redefine.tasks";
/**
 * TODO
 * Supported aggregate types:
 * 0. SCALAR: single value
 * 1. RANGE: Range of integers/char only. Can be a union of ranges
 * 2. REGION: region{lb1,lb2...lbn,ub1,ub2...ubn}:{var1,var2....varn}:{lb1<=var1, var1<=ub1,...}. Region is instantiated using values for lb1,ub1,lb2,ub2....
 * 			  An inner variable (Eg:var2 is inner to var1) can be a function of the outer variables i.e., f1(var1)<=var2<=f2(var1). This is for supporting non-rectangular polyhedra
 * 3. UNION: Union of any of the above aggregate types. Eg: {RANGE1},{RANGE2}
 */
//static string RANGE="Range";
//static string REGION="Region";
//static string UNION="Union";

class HyperOpMetadataParser {
public:
	HyperOpMetadataParser();
	virtual ~HyperOpMetadataParser();
	HyperOpInteractionGraph * parseMetadata(Module *);
};


#endif /* LIB_TARGET_RISCV_HYPEROPMETADATAPARSER_H_ */

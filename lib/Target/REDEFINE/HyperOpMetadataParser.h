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
#include "HyperOpInteractionGraph.h"

/**
 * Metadata format is as follows:
 * 0. NamedMDNode "redefine.annotations" containing the following NamedMDNode list:
 * 		0. NamedMDNode "redefine.hyperops" containing the list of HyperOps which further contains MDNode with first argument "redefine.hyperop" for each HyperOp
 * 		1. NamedMDNode "redefine.prescribesEdges" containing the list of HyperOp prescription conditions
 * 		2. NamedMDNode "redefine.producesEdges" containing the list of HyperOp data production edges
 * 		3. NamedMDNode "redefine.consumesEdges" containing the list of HyperOp data consumption edges
 */
static string REDEFINE_ANNOTATIONS = "redefine.annotations";
static string HYPEROP = "HyperOp";
static string HYPEROP_PRODUCES = "Produces";
static string HYPEROP_PRESCRIBES = "Prescribes";
static string HYPEROP_AFFINITY= "Affinity";
static string HYPEROP_START= "Start";
static string HYPEROP_END= "End";


//const string TASKS_LIST = "redefine.tasks";
/**
 * Supported aggregate types:
 * 0. SCALAR: single value
 * 1. RANGE: Range of integers/char only. Can be a union of ranges
 * 2. REGION: region{lb1,lb2...lbn,ub1,ub2...ubn}:{var1,var2....varn}:{lb1<=var1, var1<=ub1,...}. Region is instantiated using values for lb1,ub1,lb2,ub2....
 * 			  An inner variable (Eg:var2 is inner to var1) can be a function of the outer variables i.e., f1(var1)<=var2<=f2(var1). This is for supporting non-rectangular polyhedra
 * 3. UNION: Union of any of the above aggregate types. Eg: {RANGE1},{RANGE2}
 */

static string SCALAR="Scalar";
static string RANGE="Range";
static string REGION="Region";
static string UNION="Union";

class HyperOpMetadataParser {
public:
	HyperOpMetadataParser();
	virtual ~HyperOpMetadataParser();
	HyperOpInteractionGraph * parseMetadata(Module *);
};


#endif /* LIB_TARGET_RISCV_HYPEROPMETADATAPARSER_H_ */

/*
 * REDEFINESelectionDAGBuilder.h
 *
 *  Created on: 31-Aug-2015
 *      Author: kavitha
 */

#ifndef LIB_TARGET_REDEFINE_REDEFINESELECTIONDAGBUILDER_H_
#define LIB_TARGET_REDEFINE_REDEFINESELECTIONDAGBUILDER_H_

#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/SelectionDAGBuilder.h"
#include "llvm/CodeGen/FunctionLoweringInfo.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/CodeGen/Analysis.h"

using namespace llvm;

/**
 * Had to write this because visitRet had to be overridden
 */
class REDEFINESelectionDAGBuilder: public llvm::SelectionDAGBuilder {
public:
	REDEFINESelectionDAGBuilder(SelectionDAG &CurDAG, FunctionLoweringInfo &FuncInfo, CodeGenOpt::Level OptLevel) :
			SelectionDAGBuilder(CurDAG, FuncInfo, OptLevel) {

	}
	virtual ~REDEFINESelectionDAGBuilder();
	
	void visitRet(const ReturnInst &I);


};

#endif /* LIB_TARGET_REDEFINE_REDEFINESELECTIONDAGBUILDER_H_ */

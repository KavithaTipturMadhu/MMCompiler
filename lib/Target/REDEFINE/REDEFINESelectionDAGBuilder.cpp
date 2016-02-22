/*
 * REDEFINESelectionDAGBuilder.cpp
 *
 *  Created on: 31-Aug-2015
 *      Author: kavitha
 */

#include "REDEFINESelectionDAGBuilder.h"

#include "llvm/Support/Casting.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetFrameLowering.h"

using namespace llvm;

REDEFINESelectionDAGBuilder::~REDEFINESelectionDAGBuilder() {
	// TODO Auto-generated destructor stub
}

void REDEFINESelectionDAGBuilder::visitRet(const ReturnInst &I) {

}

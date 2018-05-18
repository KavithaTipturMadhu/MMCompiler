/*
 * REDEFINEUtils.cpp
 *
 *  Created on: 22-Apr-2016
 *      Author: kavitha
 */

#include "llvm/IR/REDEFINEUtils.h"
using namespace llvm;

//Returns size of the type in bytes
unsigned REDEFINEUtils::getSizeOfType(Type * type) {
	Type* objectType = type;
	if (isa<PointerType>(type)) {
		objectType = ((PointerType*) type)->getPointerElementType();
	}
	if (objectType->isAggregateType()) {
		unsigned size = 0;
		for (unsigned i = 0; i < objectType->getNumContainedTypes(); i++) {
			size += getSizeOfType(objectType->getContainedType(i));
		}
		unsigned numElements = 1;
		if (objectType->isArrayTy()) {
			numElements = objectType->getArrayNumElements();
		}
		return size * numElements;
	}
	return (32 / 8);
}

//Returns size of the type aligned by ALIGNMENT_SIZE
unsigned REDEFINEUtils::getAlignedSizeOfType(Type * type) {
	unsigned returnSize = getSizeOfType(type);
	if (returnSize % ALIGNMENT_SIZE > 0) {
		returnSize += (ALIGNMENT_SIZE-returnSize % ALIGNMENT_SIZE);
	}
	return returnSize;
}

unsigned REDEFINEUtils::getHyperOpId(HyperOp* hyperOp) {
	unsigned returnValue;
	unsigned contextFrameId = hyperOp->getContextFrame();
	unsigned crId = hyperOp->getTargetResource();
	unsigned pageNumber = contextFrameId / 52;
	contextFrameId = contextFrameId % 52;
	return (crId << 22) | (pageNumber << 12) | (contextFrameId << 6);
}

void REDEFINEUtils::print_backtrace(void)
{
        static const char start[] = "BACKTRACE ------------\n";
        static const char end[] = "----------------------\n";

        void *bt[1024];
        int bt_size;
        char **bt_syms;
        int i;

        bt_size = backtrace(bt, 1024);
        bt_syms = backtrace_symbols(bt, bt_size);
        for (i = 1; i < bt_size; i++) {
              errs()<<bt_syms[i]<<"\n";
        }
    free(bt_syms);
}

bool REDEFINEUtils::isInteger(StringRef inputString){
	const char * str = inputString.data();
	if(inputString.empty()){
		return false;
	}
	for(int i=0;i<inputString.size();i++){
		if(str[i]>'9' || str[i]<'0'){
			return false;
		}
	}
	return true;
}

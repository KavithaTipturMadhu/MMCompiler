/*
 * REDEFINEUtils.cpp
 *
 *  Created on: 22-Apr-2016
 *      Author: kavitha
 */

#include "REDEFINEUtils.h"
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
	return (objectType->getPrimitiveSizeInBits() / 8);
}

//Returns size of the type in bytes
unsigned REDEFINEUtils::getAlignedSizeOfType(Type * type) {
	unsigned returnSize = getSizeOfType(type);
	if (returnSize % ALIGNMENT_SIZE > 0) {
		returnSize += returnSize % ALIGNMENT_SIZE;
	}
	return returnSize;
}

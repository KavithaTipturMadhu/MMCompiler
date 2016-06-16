/*
 * REDEFINEUtils.h
 *
 *  Created on: 22-Apr-2016
 *      Author: kavitha
 */

#ifndef LIB_TARGET_REDEFINE_REDEFINEUTILS_H_
#define LIB_TARGET_REDEFINE_REDEFINEUTILS_H_

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"

namespace llvm {
class REDEFINEUtils {
	//Alignment is supposed to be for 8 words
	static unsigned ALIGNMENT_SIZE = 32;
public:
	static unsigned getSizeOfType(Type * type);
	//This method is to ensure that the types are aligned to 'n' word sizes to guarantee that data movement between DGM and global memory and vice-versa dont need extra packets to be processed
	static unsigned getAlignedSizeOfType(Type * type);

};
}
#endif /* LIB_TARGET_REDEFINE_REDEFINEUTILS_H_ */

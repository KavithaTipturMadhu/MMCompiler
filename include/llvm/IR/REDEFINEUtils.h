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
#include "llvm/IR/HyperOpInteractionGraph.h"
#include <execinfo.h>

namespace llvm {
class REDEFINEUtils {
	//Alignment is supposed to be for 64 words
	const static unsigned ALIGNMENT_SIZE = 256;
public:
	const static unsigned SIZE_OF_INSTRUCTION = 32;
	static unsigned getSizeOfType(Type * type);
	//This method is to ensure that the types are aligned to 'n' word sizes to guarantee that data movement between DGM and global memory and vice-versa dont need extra packets to be processed
	static unsigned getAlignedSizeOfType(Type * type);
	static unsigned getHyperOpId(HyperOp* hyperOp);
	static void print_backtrace(void);
	static bool isInteger(StringRef);
};
}
#endif /* LIB_TARGET_REDEFINE_REDEFINEUTILS_H_ */

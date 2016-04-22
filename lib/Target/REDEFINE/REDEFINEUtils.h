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
public:
	static unsigned getSizeOfType(Type * type);
};
}
#endif /* LIB_TARGET_REDEFINE_REDEFINEUTILS_H_ */

//===- ExtendedBasicBlockPass.cpp-------------------===//
#include<vector>
#include<string>
#include<list>
using namespace std;
#define DEBUG_TYPE "extendedBasicBlockPass"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/raw_ostream.h"
#include "ValueNumberTable.h"
#include "NumListTable.h"
#include "ExpressionTable.h"
#include "llvm/IR/Function.h"
using namespace llvm;

/**
 * Basic block pass to perform:
 * 1.Constant propogation
 * 2.Constant Folding
 * 3.Dead code elimination
 */
struct ExtendedBasicBlockPass: public FunctionPass {
	static char ID; // Pass identification, replacement for typeid
	ValueNumberTable valnumTable;
	NumListTable numberListTable;
	ExpressionTable expressionTable;

	ExtendedBasicBlockPass() :
			FunctionPass(ID) {
	}

	Constant * foldConstant(Instruction *instruction) {
		if (instruction->isBinaryOp()) {
			Value *firstOperand = instruction->getOperand(0);
			Value *secondOperand = instruction->getOperand(1);
			int opcode = instruction->getOpcode();
			if (!isa<Constant>(firstOperand) || !isa<Constant>(secondOperand)) {
				return 0;
			} else {
				//TODO: compare & load instructions
				Constant *firstOperandConstant = (Constant *) firstOperand;
				Constant *SecondOperandConstant = (Constant *) secondOperand;
				switch (opcode) {
				//TODO: support the following
				case Instruction::Call:
				case Instruction::PtrToInt:
				case Instruction::IntToPtr:
					return 0;
				case Instruction::Trunc:
				case Instruction::ZExt:
				case Instruction::SExt:
				case Instruction::FPTrunc:
				case Instruction::FPExt:
				case Instruction::UIToFP:
				case Instruction::SIToFP:
				case Instruction::FPToUI:
				case Instruction::FPToSI:
					return ConstantExpr::getCast(opcode, firstOperandConstant, instruction->getType());
				case Instruction::BitCast:
					return ConstantExpr::getBitCast(firstOperandConstant, instruction->getType());
				case Instruction::Add:
					return ConstantExpr::getAdd(firstOperandConstant, SecondOperandConstant);
				case Instruction::FAdd:
					return ConstantExpr::getFAdd(firstOperandConstant, SecondOperandConstant);
				case Instruction::Sub:
					return ConstantExpr::getSub(firstOperandConstant, SecondOperandConstant);
				case Instruction::FSub:
					return ConstantExpr::getFSub(firstOperandConstant, SecondOperandConstant);
				case Instruction::Mul:
					return ConstantExpr::getMul(firstOperandConstant, SecondOperandConstant);
				case Instruction::FMul:
					return ConstantExpr::getFMul(firstOperandConstant, SecondOperandConstant);
				case Instruction::UDiv:
					return ConstantExpr::getUDiv(firstOperandConstant, SecondOperandConstant);
				case Instruction::SDiv:
					return ConstantExpr::getSDiv(firstOperandConstant, SecondOperandConstant);
				case Instruction::FDiv:
					return ConstantExpr::getFDiv(firstOperandConstant, SecondOperandConstant);
				case Instruction::URem:
					return ConstantExpr::getURem(firstOperandConstant, SecondOperandConstant);
				case Instruction::SRem:
					return ConstantExpr::getSRem(firstOperandConstant, SecondOperandConstant);
				case Instruction::FRem:
					return ConstantExpr::getFRem(firstOperandConstant, SecondOperandConstant);
				case Instruction::Shl:
					return ConstantExpr::getShl(firstOperandConstant, SecondOperandConstant);
				case Instruction::LShr:
					return ConstantExpr::getLShr(firstOperandConstant, SecondOperandConstant);
				case Instruction::AShr:
					return ConstantExpr::getAShr(firstOperandConstant, SecondOperandConstant);
				case Instruction::And:
					return ConstantExpr::getAnd(firstOperandConstant, SecondOperandConstant);
				case Instruction::Or:
					return ConstantExpr::getOr(firstOperandConstant, SecondOperandConstant);
				case Instruction::Xor:
					return ConstantExpr::getXor(firstOperandConstant, SecondOperandConstant);
				default:
					return 0;
				}

			}
		}
		return 0;
	}

	//Returns equivalent expression if given expression is redundant
	Value * handleRendundancies(Instruction *instruction) {
		if (instruction->isBinaryOp()) {
			Value *firstOperand = instruction->getOperand(0);
			Value *secondOperand = instruction->getOperand(1);
			int opcode = instruction->getOpcode();
			switch (opcode) {
			case Instruction::Add:
			case Instruction::FAdd:
			case Instruction::Or:
				if (isa<Constant>(firstOperand) && ((Constant*) firstOperand)->isZeroValue()) {
					return secondOperand;
				}
				if (isa<Constant>(secondOperand) && ((Constant*) secondOperand)->isZeroValue()) {
					return firstOperand;
				}
				break;
			case Instruction::Mul:
			case Instruction::FMul:
				if (isa<Constant>(firstOperand) && ((Constant*) firstOperand)->getUniqueInteger() == 1) {
					return secondOperand;
				}
				if (isa<Constant>(secondOperand) && ((Constant*) secondOperand)->getUniqueInteger() == 1) {
					return firstOperand;
				}
				break;
			case Instruction::And:
				if (isa<Constant>(firstOperand) && ((Constant*) firstOperand)->isAllOnesValue()) {
					return secondOperand;
				}
				if (isa<Constant>(secondOperand) && ((Constant*) secondOperand)->isAllOnesValue()) {
					return firstOperand;
				}
				break;
			}

		}
		return 0;
	}

	bool visitBasicBlock(BasicBlock & BB, ValueNumberTable valnumTable, NumListTable numberListTable, ExpressionTable expressionTable) {
		bool changesMade = false;
//		ValueNumberTable valnumTable;
//		NumListTable numberListTable;
//		ExpressionTable expressionTable;

		list<Instruction *> instructionList;
		int i = 0;

		Instruction * instruction = BB.getInstList().begin();
		while (instruction && instruction != BB.getInstList().end()) {
			switch (instruction->getOpcode()) {
			case Instruction::Br:
			case Instruction::Alloca:
			case Instruction::BitCast:
				//Do nothing
				break;
			case Instruction::Store:
				//If instruction is storing a constant
				if (isa<Constant>(instruction->getOperand(0))) {
					Value *firstOperand = instruction->getOperand(0);
					Value *secondOperand = instruction->getOperand(1);
					int valueNumber = valnumTable.insert(secondOperand);
					if (valueNumber != -1) {
						numberListTable.addNewNumberInNumberList(valueNumber, secondOperand, (Constant *) firstOperand);
						expressionTable.removeInstructionsUsingOperand(secondOperand);
					}
				} else {
					Value *firstOperand = instruction->getOperand(0);
					int valueNumber = valnumTable.find(firstOperand);
					if (valueNumber > -1) {
						Constant *constant = numberListTable.getConstantValue(valueNumber);
						if (constant) {
							instruction->setOperand(0, constant);
							changesMade = true;
							Value *secondOperand = instruction->getOperand(1);
							int valueNumber = valnumTable.insert(secondOperand);
							numberListTable.addNewNumberInNumberList(valueNumber, secondOperand, constant);
						}
					} else {
						valueNumber = expressionTable.find((Instruction *) firstOperand);
						Instruction * firstOperandInstruction = (Instruction *) firstOperand;
						//Handle commutativity
						if (valueNumber == -1 && firstOperandInstruction->isCommutative()) {
							Value *firstOperand = firstOperandInstruction->getOperand(0);
							Value *secondOperand = firstOperandInstruction->getOperand(1);
							firstOperandInstruction->setOperand(0, secondOperand);
							firstOperandInstruction->setOperand(1, firstOperand);
							valueNumber = expressionTable.find(firstOperandInstruction);
						}
						if (valueNumber > -1) {
							Value * replacingInstruction = valnumTable.findInstructionWithValueNumber(valueNumber);
							if (replacingInstruction != NULL) {
								instructionList.push_back((Instruction *) instruction->getOperand(0));
								instruction->setOperand(0, replacingInstruction);
							}
						}
					}
				}
				break;
			case Instruction::Load:
				if (!isa<Constant>(instruction->getOperand(0))) {
					Value *firstOperand = instruction->getOperand(0);
					int valueNumber = valnumTable.find(firstOperand);
					Constant *c = NULL;
					if (valueNumber > -1) {
						c = numberListTable.getConstantValue(valueNumber);
						valueNumber = valnumTable.insert(instruction);
						numberListTable.addNewNumberInNumberList(valueNumber, instruction, c);
					}
				}
				break;
			default:
				bool hasVariableOperands = false;
				//Constant propogation
				for (i = 0; i < instruction->getNumOperands(); i++) {
					Value * operand = instruction->getOperand(i);
					bool isInstructionOperand = false;
					if (!isa<Constant>(operand)) {
						int valueNumber = valnumTable.find(operand);
						if (valueNumber != -1) {
							Constant * constantExpression = numberListTable.getConstantValue(valueNumber);
							if (constantExpression != NULL) {
								instruction->setOperand(i, constantExpression);
								instructionList.push_back((Instruction *) operand);
								changesMade = true;
							}
						} else if (isa<Instruction>(operand)) {
							hasVariableOperands = true;
							Instruction *instructionOperand = (Instruction *) operand;
							int expressionValueNumber = expressionTable.find(instructionOperand);
							//Handle commutative instructions
							if (expressionValueNumber == -1 && instructionOperand->getNumOperands() > 1 && instructionOperand->isCommutative()) {
								Value *firstOperand = instructionOperand->getOperand(0);
								Value *secondOperand = instructionOperand->getOperand(1);
								instructionOperand->setOperand(0, secondOperand);
								instructionOperand->setOperand(1, firstOperand);
								expressionValueNumber = expressionTable.find(instructionOperand);
							}
							if (expressionValueNumber > -1) {
								instruction->setOperand(i, valnumTable.findInstructionWithValueNumber(expressionValueNumber));
								instructionList.push_back((Instruction *) operand);
							}
						}
					}
				}

				if (hasVariableOperands) {
					Value * redundantExpressionReplacement = handleRendundancies(instruction);
					int expressionValueNumber = expressionTable.find(instruction);
					if (expressionValueNumber == -1 && instruction->getNumOperands() > 1 && instruction->isCommutative() && redundantExpressionReplacement == NULL) {
						Value *firstOperand = instruction->getOperand(0);
						Value *secondOperand = instruction->getOperand(1);
						instruction->setOperand(0, secondOperand);
						instruction->setOperand(1, firstOperand);
						expressionValueNumber = expressionTable.find(instruction);
					}
					if (expressionValueNumber == -1) {
						if (redundantExpressionReplacement == NULL) {
							expressionValueNumber = valnumTable.insert(instruction);
						} else {
							expressionValueNumber = valnumTable.insert(redundantExpressionReplacement);
						}
						expressionTable.insert(instruction, expressionValueNumber);
					} else {
						for (i = 0; i < instruction->getNumOperands(); i++) {
							Value * operand = instruction->getOperand(i);
							if (isa<Instruction>(operand)) {
								instructionList.push_back((Instruction *) operand);
								changesMade = true;
							}
						}

					}
				} else {
					//Fold constants
					Constant *foldedInstruction = foldConstant(instruction);
					if (foldedInstruction) {
						int valNum = valnumTable.insert(instruction);
						if (valNum != -1) {
							changesMade = true;
							numberListTable.addNewNumberInNumberList(valNum, instruction, (Constant *) foldedInstruction);
							instructionList.push_back(instruction);
						}
					}
				}
				break;
			}
			instruction = instruction->getNextNode();
		}

		//Reversing the list to ensure dependencies are not violated during dead code deletion
		instructionList.reverse();

		//Remove unnecessary instructions from the basic block
		for (list<Instruction*>::const_iterator it = instructionList.begin(); it != instructionList.end(); it++) {
			(*it)->eraseFromParent();
		}
		return changesMade;
	}

	virtual bool runOnFunction(Function &F) {
		list<BasicBlock *> ebbStartPointerList;
		bool changesMade = false;
		valnumTable.clear();
		expressionTable.clear();
		numberListTable.clear();

		for (Function::iterator basicBlockIterator = F.begin(), endOfFunction = F.end(); basicBlockIterator != endOfFunction; ++basicBlockIterator) {
			changesMade = changesMade || visitBasicBlock(*basicBlockIterator, valnumTable, numberListTable, expressionTable);
		}
		return changesMade;
	}
};

char ExtendedBasicBlockPass::ID = 3;
static RegisterPass<ExtendedBasicBlockPass> X("extendedBasicBlockPass", "Optimization Pass");

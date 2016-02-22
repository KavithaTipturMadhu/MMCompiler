#include<vector>
#include<string>
#include<list>
#include<map>
using namespace std;
#define DEBUG_TYPE "ConditionalConstantPropogation"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/User.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"
#include "EdgePile.h"
#include "Edge.h"
#include "Lattice.h"
using namespace llvm;

/**
 * Pass to perform SCCP
 */
struct ConditionalConstantPass: public FunctionPass {
	static char ID; // Pass identification, replacement for typeid
	EdgePile flowPile;
	EdgePile ssaPile;
	EdgePile executedFlowPile;
	map<Value *, Lattice> latticeValueMap;

	ConditionalConstantPass() :
			FunctionPass(ID) {
	}

	Constant * foldConstant(Instruction *instruction, Value *firstOperand, Value *secondOperand) {
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
			case Instruction::ICmp:
			case Instruction::FCmp:
				return ConstantExpr::getCompare(((CmpInst *) instruction)->getPredicate(), firstOperandConstant, SecondOperandConstant);
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
		return 0;
	}

	void handlePHI(BasicBlock *basicBlock, PHINode * instruction) {
		LatticeValue prevLatticeValue = UNDEFINED;
		Value *previousOutput = NULL;
		if (latticeValueMap.find(instruction) != latticeValueMap.end()) {
			prevLatticeValue = latticeValueMap.find(instruction)->second.getLatticeValue();
			previousOutput = latticeValueMap.find(instruction)->second.getInstructionValue();
		}

		LatticeValue latticeOutput = UNDEFINED;
		Value * output = previousOutput;

		for (int i = 0; i < instruction->getNumOperands(); i++) {
			LatticeValue inputLatticeValue = UNDEFINED;
			Value * inputForBlock = instruction->getOperand(i);
			if (isa<Constant>(inputForBlock)) {
				inputLatticeValue = CONSTANT;
			} else {
				BasicBlock * predecessorBlock = instruction->getIncomingBlock(i);
				if (latticeValueMap.find(inputForBlock) != latticeValueMap.end() && executedFlowPile.hasEnabledInputEdge(predecessorBlock)) {
					TerminatorInst *terminatorInstruction = predecessorBlock->getTerminator();
					bool canGetLatticeValue = true;
					if (isa<BranchInst>(terminatorInstruction) && ((BranchInst *) terminatorInstruction)->isConditional()) {
						LatticeValue preceedingInstructionLatticeValue = latticeValueMap.find(((BranchInst*) terminatorInstruction))->second.getLatticeValue();
						Value *preceedingLatticeOutput = latticeValueMap.find(((BranchInst*) terminatorInstruction))->second.getInstructionValue();
						if (preceedingInstructionLatticeValue == CONSTANT) {
							bool isConditionFalse = ((Constant *) preceedingLatticeOutput)->isZeroValue();
							if ((isConditionFalse && ((BranchInst*) terminatorInstruction)->getNumOperands() == 2 && ((BranchInst*) terminatorInstruction)->getOperand(1)->getValueID() == instruction->getParent()->getValueID())
									|| (((BranchInst *) terminatorInstruction)->getOperand(0)->getValueID() == instruction->getParent()->getValueID())) {
								canGetLatticeValue = true;
							} else {
								canGetLatticeValue = false;
							}
						}
					}
					if (canGetLatticeValue) {
						inputLatticeValue = latticeValueMap.find(inputForBlock)->second.getLatticeValue();
						inputForBlock = latticeValueMap.find(inputForBlock)->second.getInstructionValue();
					}
				}
			}
			if (inputLatticeValue < latticeOutput) {
				latticeOutput = inputLatticeValue;
				if (inputLatticeValue == CONSTANT) {
					output = inputForBlock;
				} else if (inputLatticeValue == NAC) {
					latticeOutput = NAC;
					output = instruction;
				}
			}
			if (inputLatticeValue == latticeOutput) {
				if (inputLatticeValue == CONSTANT) {
					if (ConstantExpr::getSub((Constant*) output, (Constant *) inputForBlock)->isZeroValue()) {
						latticeOutput = CONSTANT;
					} else {
						latticeOutput = NAC;
						output = instruction;
					}
				} else if (inputLatticeValue == NAC) {
					latticeOutput = NAC;
					output = instruction;
				}
			}
		}

		if (latticeValueMap.find(instruction) != latticeValueMap.end()) {
			latticeValueMap.erase(instruction);
		}

		latticeValueMap.insert(pair<Value *, Lattice>(instruction, Lattice(output, latticeOutput)));

		if (latticeOutput < prevLatticeValue) {
			value_use_iterator<User> user, end;
			if (instruction->hasNUsesOrMore(1)) {
				list<Edge *> useList;
				for (user = instruction->use_begin(), end = instruction->use_end(); user != end; ++user) {
					if (isa<Instruction>(*user) && ((Instruction *) (*user))->getParent() != instruction->getParent()) {
						TerminatorInst* terminator = instruction->getParent()->getTerminator();
						//Add all successors to flowpile
						for (int i = 0; i < terminator->getNumSuccessors(); i++) {
							if (terminator->getSuccessor(i)->getValueID() != ((Instruction *) (*user))->getParent()->getValueID()) {
								Edge *edge = new Edge(basicBlock, ((Instruction *) (*user))->getParent());
								useList.push_back(edge);
							}
						}

					}
				}

				useList.reverse();
				//Remove unnecessary instructions from the basic block
				for (list<Edge *>::const_iterator it = useList.begin(); it != useList.end(); it++) {
					ssaPile.addEdge(*it);
				}
			}
		}

	}

	void handleInstruction(BasicBlock *basicBlock, Instruction * instruction) {
		LatticeValue prevLatticeValue = UNDEFINED;
		Value *prevLatticeOutput = 0;
		bool insertToLatticeMap = true;
		if (latticeValueMap.find(instruction) != latticeValueMap.end()) {
			prevLatticeValue = latticeValueMap.find(instruction)->second.getLatticeValue();
			prevLatticeOutput = latticeValueMap.find(instruction)->second.getInstructionValue();
		}
		LatticeValue latticeOutput = UNDEFINED;
		Value *output = prevLatticeOutput;

		if (instruction->getNumOperands() == 1 && !isa<BranchInst>(instruction)) {
			Value * operand = instruction->getOperand(0);
			if (isa<Constant>(operand)) {
				output = prevLatticeOutput;
				latticeOutput = prevLatticeValue;
			}
		} else if (isa<BranchInst>(instruction)) {
			BranchInst* branch = (BranchInst *) instruction;
			if (branch->isConditional()) {
				Value * operand = instruction->getOperand(0);
				if (isa<Constant>(operand)) {
					output = prevLatticeOutput;
					latticeOutput = prevLatticeValue;
				} else if (latticeValueMap.find(operand) != latticeValueMap.end()) {
					output = latticeValueMap.find(operand)->second.getInstructionValue();
					latticeOutput = latticeValueMap.find(operand)->second.getLatticeValue();
				}
			} else {
				insertToLatticeMap = false;
			}

		} else if (instruction->getNumOperands() == 2) {
			Value *firstOperand = instruction->getOperand(0);
			Value *secondOperand = instruction->getOperand(1);
			LatticeValue firstOperandLatticeValue = UNDEFINED;
			LatticeValue secondOperandLatticeValue = UNDEFINED;

			if (isa<Constant>(firstOperand)) {
				firstOperandLatticeValue = CONSTANT;
			} else if (latticeValueMap.find(firstOperand) != latticeValueMap.end()) {
				firstOperandLatticeValue = latticeValueMap.find(firstOperand)->second.getLatticeValue();
				firstOperand = latticeValueMap.find(firstOperand)->second.getInstructionValue();
			}
			if (isa<Constant>(secondOperand)) {
				secondOperandLatticeValue = CONSTANT;
			} else if (latticeValueMap.find(secondOperand) != latticeValueMap.end()) {
				secondOperandLatticeValue = latticeValueMap.find(secondOperand)->second.getLatticeValue();
				secondOperand = latticeValueMap.find(secondOperand)->second.getInstructionValue();
			}

			if (firstOperandLatticeValue == CONSTANT && secondOperandLatticeValue == CONSTANT) {
				output = foldConstant(instruction, firstOperand, secondOperand);
				if (output) {
					latticeOutput = CONSTANT;
				}
			} else {
				latticeOutput = (firstOperandLatticeValue < secondOperandLatticeValue) ? firstOperandLatticeValue : secondOperandLatticeValue;
			}
		}

		if (latticeValueMap.find(instruction) != latticeValueMap.end()) {
			latticeValueMap.erase(instruction);
		}
		if (!output) {
			output = instruction;
		}

		if (insertToLatticeMap) {
			latticeValueMap.insert(pair<Value *, Lattice>(instruction, Lattice(output, latticeOutput)));
		}

		if (latticeOutput < prevLatticeValue) {
			if (instruction->getOpcode() == Instruction::Br) {
				BranchInst* branchInstruction = (BranchInst *) instruction;
				Value *compareOperator = NULL;
				if (branchInstruction->isConditional()) {
					compareOperator = branchInstruction->getCondition();
				}
				bool hasFalseBranch = instruction->getNumOperands() > 2 ? true : false;
				latticeOutput = UNDEFINED;
				if (compareOperator != NULL && (isa<Constant>(compareOperator) || isa<Instruction>(compareOperator))) {
					if (isa<Constant>(compareOperator)) {
						latticeOutput = CONSTANT;
						output = compareOperator;
					} else if (latticeValueMap.find(compareOperator)->second.getLatticeValue() == CONSTANT) {
						latticeOutput = CONSTANT;
						output = latticeValueMap.find(compareOperator)->second.getInstructionValue();
					}
				}
				switch (latticeOutput) {
				case NAC:
				case UNDEFINED:
					flowPile.addEdge(new Edge(basicBlock, basicBlock->getTerminator()->getSuccessor(0)));
					if (hasFalseBranch) {
						flowPile.addEdge(new Edge(basicBlock, basicBlock->getTerminator()->getSuccessor(1)));
					}
					break;
				default:
					if (isa<Constant>(output)) {
						if (!((Constant *) output)->isZeroValue()) {
							flowPile.addEdge(new Edge(basicBlock, basicBlock->getTerminator()->getSuccessor(0)));
						} else if (hasFalseBranch) {
							flowPile.addEdge(new Edge(basicBlock, basicBlock->getTerminator()->getSuccessor(1)));
						}
					}
					break;
				}
			} else {
				value_use_iterator<User> user, end;
				for (user = instruction->use_begin(), end = instruction->use_end(); user != end; ++user) {
					if (isa<Instruction>(*user) && ((Instruction *) (*user))->getParent() != instruction->getParent()) {
						TerminatorInst* terminator = instruction->getParent()->getTerminator();
						//Add all successors to flowpile
						for (int i = 0; i < terminator->getNumSuccessors(); i++) {
							if (terminator->getSuccessor(i)->getValueID() != ((Instruction *) (*user))->getParent()->getValueID()) {
								Edge *edge = new Edge(basicBlock, ((Instruction *) (*user))->getParent());
								ssaPile.addEdge(edge);
							}
						}

					}
				}
			}
		}
	}

	void visitBasicBlock(BasicBlock *basicBlock, bool ssaEdgeTarget) {
		bool canVisit = true;
		if (ssaEdgeTarget) {
			//if ssa edge, visit if atleast one predecessor is enabled
			canVisit = executedFlowPile.hasEnabledInputEdge(basicBlock);
		} else {
			//If flowedge, visit only once : If more than one inputs are enabled, don't visit
			canVisit = !executedFlowPile.hasMoreThanOneEnabledInputs(basicBlock);
		}

		Instruction * instruction = basicBlock->getInstList().begin();
		while (instruction && instruction != basicBlock->getInstList().end()) {
			if (isa<PHINode>(instruction)) {
				handlePHI(basicBlock, (PHINode *) instruction);
			} else if (canVisit) {
				handleInstruction(basicBlock, instruction);
			}
			instruction = instruction->getNextNode();
		}
	}

	virtual bool runOnFunction(Function &F) {
		flowPile.clear();
		ssaPile.clear();
		executedFlowPile.clear();

		BasicBlock * basicBlock = F.getBasicBlockList().begin();
		//Visit the first basic block
		visitBasicBlock(basicBlock, false);
		TerminatorInst * terminator = basicBlock->getTerminator();

		//Add all successors to flowpile
		for (int i = 0; i < terminator->getNumSuccessors(); i++) {
			if (!isa<BranchInst>(terminator) || !(((BranchInst *) terminator)->isConditional())) {
				flowPile.addEdge(new Edge(basicBlock, terminator->getSuccessor(i)));
			}
		}

		while (!flowPile.isEmpty() || !ssaPile.isEmpty()) {
			while (!flowPile.isEmpty()) {
				Edge *executableEdge = flowPile.getNextExecutableEdge();
				if (executedFlowPile.isEmpty() || !executedFlowPile.hasEdge(executableEdge)) {
					executableEdge->markEdgeExecutable();
					executedFlowPile.addEdge(executableEdge);
					BasicBlock* targetBasicBlock = executableEdge->getTarget();
					visitBasicBlock(targetBasicBlock, false);
					if (targetBasicBlock && targetBasicBlock->getTerminator() && targetBasicBlock->getTerminator()->getNumSuccessors() == 1) {
						flowPile.addEdge(new Edge(targetBasicBlock, targetBasicBlock->getTerminator()->getSuccessor(0)));
					}
				}
			}

			while (!ssaPile.isEmpty()) {
				Edge *executableEdge = ssaPile.getNextExecutableEdge();
				if (executableEdge) {
					BasicBlock* targetBasicBlock = executableEdge->getTarget();
					if (targetBasicBlock) {
						visitBasicBlock(targetBasicBlock, true);
					}
				}
			}
		}
		bool madeChanges = false;
		list<Instruction *> deleteInstructionList;
		for (Function::iterator BB = F.begin(), E = F.end(); BB != E; ++BB) {
			for (BasicBlock::iterator basicBlockIter = (*BB).begin(); basicBlockIter != (*BB).end(); ++basicBlockIter) {
				LatticeValue instructionLatticeValue = latticeValueMap.find((Value *) basicBlockIter)->second.getLatticeValue();
				Value * instructionValue = latticeValueMap.find((Value *) basicBlockIter)->second.getInstructionValue();
				if (instructionLatticeValue == CONSTANT && isa<Constant>(instructionValue)) {
					if (((Instruction *) basicBlockIter)->hasNUsesOrMore(1)) {
						latticeValueMap.erase((Instruction *) basicBlockIter);
						((Instruction *) basicBlockIter)->replaceAllUsesWith(instructionValue);
						if (!((Instruction *) basicBlockIter)->hasNUsesOrMore(1)) {
							deleteInstructionList.push_back((Instruction *) basicBlockIter);
						}
						madeChanges = true;
					}
				} else if (((Instruction *) basicBlockIter)->getNumOperands() == 2) {
					Constant * constantValue = foldConstant((Instruction *) basicBlockIter, ((Instruction *) basicBlockIter)->getOperand(0), ((Instruction *) basicBlockIter)->getOperand(1));
					if (constantValue) {
						if (((Instruction *) basicBlockIter)->hasNUsesOrMore(1)) {
							((Instruction *) basicBlockIter)->replaceAllUsesWith(constantValue);
						}
						deleteInstructionList.push_back((Instruction *) basicBlockIter);
						madeChanges = true;
					}
				}
			}
		}

		//Remove unnecessary instructions from the basic block
		for (list<Instruction*>::const_iterator it = deleteInstructionList.begin(); it != deleteInstructionList.end(); it++) {
			(*it)->eraseFromParent();
		}
		return madeChanges;
	}
}
;

char ConditionalConstantPass::ID = 2;
static RegisterPass<ConditionalConstantPass> X("ConditionalConstantPass", "Optimization Pass for conditional constant propogation");


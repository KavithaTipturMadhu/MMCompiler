//void method() {
//	//Add dependences across HyperOps which access the same global
//	//TODO As of now, the dependence types are not treated differently; RAR dependences needn't translate to chaining of HyperOps but do, for now
//	Function* originalFunction = const_cast<Function*>(createdHyperOpAndOriginalBasicBlockAndArgMap[createdFunction].first.front()->getParent());
//	//Check the reaching definition of the global reference object to this HyperOp
//	for (HyperOpArgumentList::iterator hyperOpArgItr = hyperOpArguments.begin(); hyperOpArgItr != hyperOpArguments.end(); hyperOpArgItr++) {
//		HyperOpArgumentType type = hyperOpArgItr->second;
//		if (type == GLOBAL_REFERENCE) {
//			//check the reaching definitions of the value
//			Value* argValue = hyperOpArgItr->first.front();
//			//Check the uses in BasicBlocks that are predecessors and use argValue
//			list<BasicBlock*> basicBlocksWithDefinitions;
//			//Find uses of argValue in the basic blocks of created function
//			for (Function::iterator originalBBItr = originalFunction->begin(); originalBBItr != originalFunction->end(); originalBBItr++) {
//				BasicBlock* originalBB = &*originalBBItr;
//				for (BasicBlock::iterator instrItr = originalBB->begin(); instrItr != originalBB->end(); instrItr++) {
//					Instruction* instr = instrItr;
//					if (isa < StoreInst > (instr) && ((StoreInst*) instr)->getOperand(0) == argValue && find(basicBlocksWithDefinitions.begin(), basicBlocksWithDefinitions.end(), originalBB) == basicBlocksWithDefinitions.end()) {
//						//Check if the store instruction is reachable to any of the uses of the argument in the accumulated bb list
//						bool originalBBHasDefinition = false;
//						for (list<Value*>::iterator phiItr = hyperOpArgItr->first.begin(); phiItr != hyperOpArgItr->first.end(); phiItr++) {
//							for (Value::use_iterator useItr = (*phiItr)->use_begin(); useItr != (*phiItr)->use_end(); useItr++) {
//								User* user = *useItr;
//								list<BasicBlock*> visitedBasicBlocks;
//								if (find(accumulatedOriginalBasicBlocks.begin(), accumulatedOriginalBasicBlocks.end(), ((Instruction*) user)->getParent()) != accumulatedOriginalBasicBlocks.end() && pathExistsInCFG(originalBB, ((Instruction*) user)->getParent(), visitedBasicBlocks)) {
//									originalBBHasDefinition = true;
//									break;
//								}
//							}
//							if (originalBBHasDefinition) {
//								break;
//							}
//						}
//						if (originalBBHasDefinition) {
//							basicBlocksWithDefinitions.push_back(originalBB);
//						}
//						break;
//					}
//				}
//			}
//
//			list<BasicBlock*> discardList;
//			for (list<BasicBlock*>::iterator defBBItr = basicBlocksWithDefinitions.begin(); defBBItr != basicBlocksWithDefinitions.end(); defBBItr++) {
//				BasicBlock* defBB = *defBBItr;
//				for (list<BasicBlock*>::iterator secDefBBItr = basicBlocksWithDefinitions.begin(); secDefBBItr != basicBlocksWithDefinitions.end(); secDefBBItr++) {
//					BasicBlock* secDefBB = *secDefBBItr;
//					list<BasicBlock*> visitedBasicBlocks;
//					if (secDefBB != defBB && (pathExistsInCFG(defBB, secDefBB, visitedBasicBlocks) || find(discardList.begin(), discardList.end(), secDefBB) != discardList.end())) {
//						discardList.push_back(defBB);
//					}
//				}
//			}
//
//			for (list<BasicBlock*>::iterator defBBItr = basicBlocksWithDefinitions.begin(); defBBItr != basicBlocksWithDefinitions.end(); defBBItr++) {
//				if (find(discardList.begin(), discardList.end(), *defBBItr) == discardList.end()) {
//					for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedOriginalBasicBlocks.begin(); accumulatedBBItr != accumulatedOriginalBasicBlocks.end(); accumulatedBBItr++) {
//						Instruction* cloneInstruction = (Instruction*) originalToClonedInstMap.find(&(*defBBItr)->front())->second;
//						list<BasicBlock*> visitedBasicBlocks;
//						if (pathExistsInCFG(*defBBItr, *accumulatedBBItr, visitedBasicBlocks) && find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), cloneInstruction->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()) {
//							//Add a predicate between the source and the target function
//							//Label the instruction with predicates metadata
//							Value * values[1];
//							values[0] = funcAnnotation;
//							MDNode* newPredicateMetadata = MDNode::get(ctxt, values);
//							vector<Value*> metadataList;
//							metadataList.push_back(newPredicateMetadata);
//							ArrayRef<Value*> metadataRef(metadataList);
//							MDNode * predicatesRelation = MDNode::get(ctxt, metadataRef);
//							//Allocated a location and store 1 there
//							AllocaInst* ai = new AllocaInst(Type::getInt1Ty(ctxt));
//							ai->setAlignment(4);
//							Instruction* retInstOfProducer = retInstMap.find(const_cast<Function*>(cloneInstruction->getParent()->getParent()))->second;
//							ai->insertBefore(retInstOfProducer->getParent()->getParent()->getEntryBlock().getFirstInsertionPt());
//							StoreInst* storeInst = new StoreInst(ConstantInt::get(ctxt, APInt(1, 1)), ai);
//							storeInst->setAlignment(4);
//							storeInst->insertBefore(retInstOfProducer);
//							ai->setMetadata(HYPEROP_CONTROLLED_BY, predicatesRelation);
//							addedParentsToCurrentHyperOp.push_back(cloneInstruction->getParent()->getParent());
//							list < pair<Function*, Instruction*> > addedJumpSources;
//							if (createdFunctionAndUnconditionalJumpSources.find(createdFunction) != createdFunctionAndUnconditionalJumpSources.end()) {
//								addedJumpSources = createdFunctionAndUnconditionalJumpSources[createdFunction];
//								createdFunctionAndUnconditionalJumpSources.erase(createdFunction);
//							}
//							addedJumpSources.push_back(make_pair(cloneInstruction->getParent()->getParent(), ai));
//							createdFunctionAndUnconditionalJumpSources[createdFunction] = addedJumpSources;
//						}
//					}
//				}
//			}
//		}
//	}
//}

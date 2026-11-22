//
//  LICM.cpp
//  uscc
//
//  Implements basic loop invariant code motion
//
//---------------------------------------------------------
//  Copyright (c) 2014, Sanjay Madhav
//  All rights reserved.
//
//  This file is distributed under the BSD license.
//  See LICENSE.TXT for details.
//---------------------------------------------------------
#include "Passes.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#pragma clang diagnostic pop

using namespace llvm;

namespace uscc
{
namespace opt
{

bool LICM::runOnLoop(llvm::Loop *L, llvm::LPPassManager &LPM)
{
	mChanged = false;

	// PA6: Implement

	mCurrLoop = L;

	mLoopInfo = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

	mDomTree = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();

	auto header = mCurrLoop->getHeader();
	auto domTreeNode = mDomTree->getNode(header);
	hoistPreOrder(domTreeNode);

	return mChanged;
}

bool LICM::isSafeToHoistInstr(llvm::Instruction* inst) const{
	/*
	 three required checls:
	 mcurrLoop->hasLoopInvariantOperands(I) is true
	 isSafeToSpeculativelyExecute(I) is true
	 isa<BinaryOperator, CastInst, SelectInst, GetElementPtrInst, CmpInst>(I) is true
	*/

	return mCurrLoop->hasLoopInvariantOperands(inst) && isSafeToSpeculativelyExecute(inst) && isa<BinaryOperator, CastInst, SelectInst, GetElementPtrInst, CmpInst>(inst);
}

void LICM::hoistInstr(llvm::Instruction* inst){
	BasicBlock* prehader = mCurrLoop->getLoopPreheader();
	if(prehader == nullptr){
		return;
	}

	inst->moveBefore((prehader->getTerminator())->getIterator());

	mChanged = true;
}

void LICM::hoistPreOrder(llvm::DomTreeNode* node){
	BasicBlock* block = node->getBlock();

	if(mLoopInfo->getLoopFor(block) == mCurrLoop){
		auto it = block->begin();
		while(it != block->end()){
			Instruction* currentInst = &*it;
			it++;
			if(isSafeToHoistInstr(currentInst)){
				hoistInstr(currentInst);
			}
		}
	}

	for(auto* child : *node){
		hoistPreOrder(child);
	}
}

void LICM::getAnalysisUsage(AnalysisUsage &Info) const
{
	// PA6: Implement
	// LICM does not modify the CFG
	Info.setPreservesCFG();
	// Execute after dead blocks have been removed
	Info.addRequired<DeadBlocks>();
	// Use the built-in Dominator tree and loop info passes
	Info.addRequired<DominatorTreeWrapperPass>();
	Info.addRequired<LoopInfoWrapperPass>();
}

} // opt
} // uscc

char uscc::opt::LICM::ID = 0;

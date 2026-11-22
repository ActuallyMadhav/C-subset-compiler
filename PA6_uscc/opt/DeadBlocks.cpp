//
//  DeadBlocks.cpp
//  uscc
//
//  Implements Dead Block Removal optimization pass.
//  This removes blocks from the CFG which are unreachable.
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
#include <llvm/IR/CFG.h>
#include <llvm/ADT/DepthFirstIterator.h>
#pragma clang diagnostic pop
#include <set>

using namespace llvm;

namespace uscc
{
namespace opt
{
	
bool DeadBlocks::runOnFunction(Function& F)
{
	bool changed = false;
	
	// PA6: Implement
	// we are going to do a dfs so we need a set of visited nodes (basic blocks)
	df_iterator_default_set<BasicBlock*> visited;

	// actual dfs starts with a for loop. loop is empty because we just need to iterate through all blocks and they are added to visited set by the df_ext functions
	for(auto it = df_ext_begin(&F.getEntryBlock(), visited); it != df_ext_end(&F.getEntryBlock(), visited); it++){
		// empty
	}
    
	std::set<BasicBlock*> cannotBeReached;

	// we need to populate the set of nodes that cannot be reached
	for(BasicBlock& bb : F){
		if(!visited.count(&bb)){
			cannotBeReached.insert(&bb);
		}
	}

	// now we need to clean up the successors of the dead blocks
	for(BasicBlock* bb : cannotBeReached){
		for(BasicBlock* successor : successors(bb)){
			successor->removePredecessor(bb);	// remove connection from predecessors for each successor in cannotBeReached
		}
	}

	for(BasicBlock* bb : cannotBeReached){
		bb->dropAllReferences();
	}

	for(BasicBlock* bb: cannotBeReached){
		bb->eraseFromParent();
	}

	if(!cannotBeReached.empty()){
		changed = true;
	}
	else{
		changed = false;
	}

	return changed;
}
	
void DeadBlocks::getAnalysisUsage(AnalysisUsage& Info) const
{
	// PA6: Implement
	Info.addRequired<ConstantBranch>();
}

} // opt
} // uscc

char uscc::opt::DeadBlocks::ID = 0;

//
//  ContantBranch.cpp
//  uscc
//
//  Implements Constant Branch Folding opt pass.
//  This converts conditional branches on constants to
//  unconditional branches.
//
//---------------------------------------------------------
//  Copyright (c) 2014, Sanjay Madhav
//  All rights reserved.
//
//  This file is distributed under the BSD license.
//  See LICENSE.TXT for details.
//---------------------------------------------------------
#include "Passes.h"
#include "SSABuilder.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#pragma clang diagnostic pop
#include <set>

using namespace llvm;

namespace uscc
{
namespace opt
{
	
bool ConstantBranch::runOnFunction(Function& F)
{
	bool changed = false;
	
	// PA6: Implement
	// copied loop from ConstantOpps.cpp
	// Make a set that contains the instructions we'll remove
	std::set<BranchInst*> removeSet;
	
	// Loop through each block
	Function::iterator blockIter = F.begin();
	while (blockIter != F.end())
	{
		// Loop through instructions in block
		BasicBlock::iterator instrIter = blockIter->begin();
		while (instrIter != blockIter->end())
		{
			// Is this conditional branch on a constant
			if(BranchInst* branch = dyn_cast<BranchInst>(instrIter)){
				if(branch->isConditional() && isa<ConstantInt>(branch->getCondition())){	// need to check if its conditional girst
					removeSet.insert(branch);
				}
			}
			++instrIter;
		}
		++blockIter;
	}

	for(auto branch : removeSet){
		auto condition = dyn_cast<ConstantInt>(branch->getCondition());
		BasicBlock* winner = nullptr;
		BasicBlock* loser = nullptr;

		if(condition->isOne()){
			winner = branch->getSuccessor(0);
			loser = branch->getSuccessor(1);
		}
		else{
			winner = branch->getSuccessor(1);
			loser = branch->getSuccessor(0);
		}

		BranchInst::Create(winner, branch->getIterator());
		loser->removePredecessor(branch->getParent());

		branch->eraseFromParent();
	}
		
	if(removeSet.empty()){
		changed = false;
	}
	else{
		changed = true;
	}

	return changed;
}

void ConstantBranch::getAnalysisUsage(AnalysisUsage& Info) const
{
	// PA6: Implement
	Info.addRequired<ConstantOps>();
}
	
} // opt
} // uscc

char uscc::opt::ConstantBranch::ID = 0;

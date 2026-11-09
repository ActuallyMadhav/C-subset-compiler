
//
//  SSABuilder.cpp
//  uscc
//
//  Implements SSABuilder class
//
//---------------------------------------------------------
//  Copyright (c) 2014, Sanjay Madhav
//  All rights reserved.
//
//  This file is distributed under the BSD license.
//  See LICENSE.TXT for details.
//---------------------------------------------------------

#include "SSABuilder.h"
#include "../parse/Symbols.h"
#include <cstdint>
#include <sys/types.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#include <llvm/IR/Value.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Constants.h>
#pragma clang diagnostic pop

#include <list>
#include <algorithm>
#include <vector>


using namespace uscc::opt;
using namespace uscc::parse;
using namespace llvm;

// Called when a new function is started to clear out all the data
void SSABuilder::reset()
{
	// PA5: Implement
	for(const auto& varDef : mVarDefs){
		delete varDef.second;
	}
	mVarDefs.clear();

	for(const auto& phis : mIncompletePhis){
		delete phis.second;
	}	
	mIncompletePhis.clear();

	mSealedBlocks.clear();
}

// For a specific variable in a specific basic block, write its value
void SSABuilder::writeVariable(Identifier* var, BasicBlock* block, Value* value)
{
	// PA5: Implement
	SubMap* toWrite = mVarDefs[block];
	(*toWrite)[var] = value;
}

// Read the value assigned to the variable in the requested basic block
// Will recursively search predecessor blocks if it was not written in this block
Value* SSABuilder::readVariable(Identifier* var, BasicBlock* block)
{
	// PA5: Implement
	SubMap* toRead = mVarDefs[block];

	auto locationIfDefined = toRead->find(var);

	if(locationIfDefined != toRead->end()){
		return locationIfDefined->second;
	}
	return readVariableRecursive(var, block);
}

// This is called to add a new block to the maps
void SSABuilder::addBlock(BasicBlock* block, bool isSealed /* = false */)
{
	// PA5: Implement
	SubMap* newSubMap = new SubMap();
	mVarDefs.insert({block, newSubMap});

	SubPHI* newSubPhi = new SubPHI();
	mIncompletePhis.insert({block, newSubPhi});

	if(isSealed){
		sealBlock(block);
	}
}

// This is called when a block is "sealed" which means it will not have any
// further predecessors added. It will complete any PHI nodes (if necessary)
void SSABuilder::sealBlock(llvm::BasicBlock* block)
{
	// PA5: Implement
	SubPHI* toSeal = mIncompletePhis[block];
	for(auto& node : *toSeal){
		addPhiOperands(node.first, node.second);
	}
	toSeal->clear();
	mSealedBlocks.insert(block);
}

// Recursively search predecessor blocks for a variable
Value* SSABuilder::readVariableRecursive(Identifier* var, BasicBlock* block)
{
	Value* retVal = nullptr;

	// PA5: Implement
	llvm::Type* phiType = var->llvmType(block->getContext());
	auto phiInsertLocatoin = block->getFirstNonPHIIt();

	BasicBlock* predecessor = block->getSinglePredecessor();	// if block has zero or more than 1 predecessor then this is nullptr

	if(!mSealedBlocks.count(block)){	// block is not sealed
		PHINode* phi = llvm::PHINode::Create(phiType, 0, "Phi", phiInsertLocatoin);
		(*mIncompletePhis[block])[var] = phi;
		retVal = phi;
	}
	else if(predecessor != nullptr){
		retVal = readVariable(var, predecessor);
	}
	else{
		PHINode* phi = llvm::PHINode::Create(phiType, 0, "Phi", phiInsertLocatoin);
		writeVariable(var, block, phi);
		retVal = addPhiOperands(var, phi);
	}

	writeVariable(var, block, retVal);

	return retVal;
}

// Adds phi operands based on predecessors of the containing block
Value* SSABuilder::addPhiOperands(Identifier* var, PHINode* phi)
{
	// PA5: Implement
	BasicBlock* parent = phi->getParent();

	for(const auto& predecessor : predecessors(parent)){
		phi->addIncoming(readVariable(var, predecessor), predecessor);
	}
	return tryRemoveTrivialPhi(phi);
}

// Removes trivial phi nodes
Value* SSABuilder::tryRemoveTrivialPhi(llvm::PHINode* phi)
{
	Value* same = nullptr;

	// PA5: Implement
	for(uint32_t i = 0; i < phi->getNumIncomingValues(); i++){
		auto operand = phi->getIncomingValue(i);

		if(operand == same || operand == phi){
			continue;
		}
		else if(same != nullptr){
			return phi;
		}
		else{
			same = operand;
		}
	}

	if(same == nullptr){
		same = UndefValue::get(phi->getType());
	}

	std::vector<PHINode*> phis;
	for(const auto& node : phi->users()){
		if(isa<PHINode>(node) && node != phi){
			auto nodeCastAsPhi = cast<PHINode>(node);
			if(std::find(phis.begin(), phis.end(), nodeCastAsPhi) == phis.end()){
				phis.push_back(nodeCastAsPhi);
			}
		}
	}

	phi->replaceAllUsesWith(same);
	for(auto& varDef : mVarDefs){
		for(auto& x : *varDef.second){
			if(x.second == phi){
				x.second = same;
			}
		}
	}
	phi->eraseFromParent();

	for(PHINode* node : phis){
		tryRemoveTrivialPhi(node);
	}

	return same;
}

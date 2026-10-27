//
//  ASTNodes.cpp
//  uscc
//
//  Implements the emitIR function for every AST node.
//
//---------------------------------------------------------
//  Copyright (c) 2014, Sanjay Madhav
//  All rights reserved.
//
//  This file is distributed under the BSD license.
//  See LICENSE.TXT for details.
//---------------------------------------------------------

#include "ASTNodes.h"
#include "Emitter.h"
#include "Symbols.h"
#include <cstddef>
#include <memory>

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
#include <llvm/IR/Intrinsics.h>
#pragma clang diagnostic pop

#include <vector>
#include <map>

using namespace uscc::parse;
using namespace llvm;

#define AST_EMIT(a) llvm::Value* a::emitIR(CodeContext& ctx) noexcept

// Program/Functions
AST_EMIT(ASTProgram)
{
	ctx.mModule = new Module("main", ctx.mGlobal);

	// Write the global string table
	ctx.mStrings.emitIR(ctx);

	// Emit declaration for stdlib "printf", if we need it
	if (ctx.mPrintfIdent != nullptr)
	{
		std::vector<llvm::Type*> printfArgs;
		printfArgs.push_back(llvm::PointerType::getUnqual(ctx.mGlobal));

		FunctionType* printfType = FunctionType::get(llvm::Type::getInt32Ty(ctx.mGlobal),
													 printfArgs, true);

		Function* func = Function::Create(printfType, GlobalValue::LinkageTypes::ExternalLinkage,
										  "printf", ctx.mModule);
		func->setCallingConv(CallingConv::C);

		// Map the printf ident to this function
		ctx.mPrintfIdent->setAddress(func);
	}

	// Emit code for all the functions
	for (auto f : mFuncs)
	{
		f->emitIR(ctx);
	}
	// A program actually doesn't have a value to return, since everything
	// is stored in Module
	return nullptr;
}

AST_EMIT(ASTFunction)
{
	FunctionType* funcType = nullptr;

	// First get the return type (there's only three choices)
	llvm::Type* retType = nullptr;
	if (mReturnType == Type::Int)
	{
		retType = llvm::Type::getInt32Ty(ctx.mGlobal);
	}
	else if (mReturnType == Type::Char)
	{
		retType = llvm::Type::getInt8Ty(ctx.mGlobal);
	}
	else
	{
		retType = llvm::Type::getVoidTy(ctx.mGlobal);
	}

	if (mArgs.size() == 0)
	{
		funcType = FunctionType::get(retType, false);
	}
	else
	{

		std::vector<llvm::Type*> args;
		for (auto arg : mArgs)
		{
			args.push_back(arg->getIdent().llvmType(ctx.mGlobal));
		}

		funcType = FunctionType::get(retType, args, false);
	}

	// Create the function, and make it the current one
	ctx.mFunc = Function::Create(funcType,
								 GlobalValue::LinkageTypes::ExternalLinkage,
								 mIdent.getName(), ctx.mModule);

	// Now that we have a new function, reset our SSA builder
	ctx.mSSA.reset();

	// Map the ident to this function
	mIdent.setAddress(ctx.mFunc);

	// Create the entry basic block
	ctx.mBlock = BasicBlock::Create(ctx.mGlobal, "entry", ctx.mFunc);
	// Add and seal this block
	ctx.mSSA.addBlock(ctx.mBlock, true);

	// If we have arguments, we need to set the name/value of them
	if (mArgs.size() > 0)
	{
		Function::arg_iterator iter = ctx.mFunc->arg_begin();
		Function::arg_iterator end = ctx.mFunc->arg_end();
		int i = 0;
		while (iter != end)
		{
			Identifier& argIdent = mArgs[i]->getIdent();
			iter->setName(argIdent.getName());

			// PA4: Remove the setAddress call
			// (Technically, iter actually has the value of the
			// arg, not its address...but we will use the address
			// member for this value)
			argIdent.setAddress(iter);

			// PA4: Write to this identifier
			// argIdent.writeTo(ctx, iter);

			++i;
			++iter;
		}
	}

	ctx.mFunc->setCallingConv(CallingConv::C);

	// Add all the declarations for variables created in this function
	mScopeTable.emitIR(ctx);

	// Now emit the body
	mBody->emitIR(ctx);

	return ctx.mFunc;
}

AST_EMIT(ASTArgDecl)
{
	// This node actually doesn't have anything to emit
	return nullptr;
}

llvm::Value* ASTArraySub::emitLinearIndex(CodeContext& ctx) noexcept
{
	Value* retVal = nullptr;

	// PA3: Implement (Array index)
	std::vector<Value*> vals;
	for(auto& expr : mExprs){
		vals.push_back(expr->emitIR(ctx));
	}

	IRBuilder<> build(ctx.mBlock);

	retVal = vals[0];

	for(size_t i = 1; i < vals.size(); i++){
		auto extentConst = ConstantInt::get(llvm::Type::getInt32Ty(ctx.mGlobal), mIdent.getExtent(i));
		retVal = build.CreateMul(retVal, extentConst);
		retVal = build.CreateAdd(retVal, vals[i]);
	}

	return retVal;
}

AST_EMIT(ASTArraySub)
{
	Value* arrayIdx = emitLinearIndex(ctx);

	// This address should already be saved
	Value* addr = mIdent.readFrom(ctx);

	// GEP from the array address
	IRBuilder<> build(ctx.mBlock);
	llvm::Type* elemTy = mIdent.llvmElementType(ctx.mGlobal);

	return build.CreateInBoundsGEP(elemTy, addr, arrayIdx);
}

// Expressions

AST_EMIT(ASTBadExpr)
{
	// This node will never be emitted
	return nullptr;
}

AST_EMIT(ASTLogicalAnd)
{
	// This is extremely similar to logical or

	// Create the block for the RHS
	BasicBlock* rhsBlock = BasicBlock::Create(ctx.mGlobal, "and.rhs", ctx.mFunc);
	// Add the rhs block to SSA (not sealed)
	ctx.mSSA.addBlock(rhsBlock);

	// In both "true" and "false" condition, we'll jump to and.end
	// This is because we'll insert a phi node that assume false
	// if the and.end jump was from the lhs block
	BasicBlock* endBlock = BasicBlock::Create(ctx.mGlobal, "and.end", ctx.mFunc);
	// Also not sealed
	ctx.mSSA.addBlock(endBlock);

	// Now generate the LHS
	Value* lhsVal = mLHS->emitIR(ctx);

	BasicBlock* lhsBlock = ctx.mBlock;

	// Add the branch to the end of the LHS
	{
		IRBuilder<> build(ctx.mBlock);
		// We can assume it WILL be an i32 here
		// since it'd have been zero-extended otherwise
		lhsVal = build.CreateICmpNE(lhsVal, ctx.mZero, "tobool");
		build.CreateCondBr(lhsVal, rhsBlock, endBlock);
	}

	// rhsBlock should now be sealed
	ctx.mSSA.sealBlock(rhsBlock);

	// Code should now be generated in the RHS block
	ctx.mBlock = rhsBlock;
	Value* rhsVal = mRHS->emitIR(ctx);

	// This is the final RHS block (for the phi node)
	rhsBlock = ctx.mBlock;

	// Add the branch and the end of the RHS
	{
		IRBuilder<> build(ctx.mBlock);
		rhsVal = build.CreateICmpNE(rhsVal, ctx.mZero, "tobool");

		// We do an unconditional branch because the phi mode will handle
		// the correct value
		build.CreateBr(endBlock);
	}

	// endBlock should now be sealed
	ctx.mSSA.sealBlock(endBlock);

	ctx.mBlock = endBlock;

	IRBuilder<> build(ctx.mBlock);

	// Figure out the value to zext
	Value* zextVal = nullptr;

	// If rhs is not also false, we need to make a phi
	if (rhsVal != ConstantInt::getFalse(ctx.mGlobal))
	{
		PHINode* phi = build.CreatePHI(llvm::Type::getInt1Ty(ctx.mGlobal), 2);
		// If we came from the lhs, it had to be false
		phi->addIncoming(ConstantInt::getFalse(ctx.mGlobal), lhsBlock);
		phi->addIncoming(rhsVal, rhsBlock);
		zextVal = phi;
	}
	else
	{
		zextVal = ConstantInt::getFalse(ctx.mGlobal);
	}

	return build.CreateZExt(zextVal, llvm::Type::getInt32Ty(ctx.mGlobal));
}

AST_EMIT(ASTLogicalOr)
{
	// Create the block for the RHS
	BasicBlock* rhsBlock = BasicBlock::Create(ctx.mGlobal, "lor.rhs", ctx.mFunc);
	// Add the rhs block to SSA (not sealed)
	ctx.mSSA.addBlock(rhsBlock);

	// In both "true" and "false" condition, we'll jump to lor.end
	// This is because we'll insert a phi node that assume true
	// if the lor.end jump was from the lhs block
	BasicBlock* endBlock = BasicBlock::Create(ctx.mGlobal, "lor.end", ctx.mFunc);
	// Also not sealed
	ctx.mSSA.addBlock(endBlock);

	// Now generate the LHS
	Value* lhsVal = mLHS->emitIR(ctx);

	BasicBlock* lhsBlock = ctx.mBlock;

	// Add the branch to the end of the LHS
	{
		IRBuilder<> build(ctx.mBlock);
		// We can assume it WILL be an i32 here
		// since it'd have been zero-extended otherwise
		lhsVal = build.CreateICmpNE(lhsVal, ctx.mZero, "tobool");
		build.CreateCondBr(lhsVal, endBlock, rhsBlock);
	}

	// rhsBlock should now be sealed
	ctx.mSSA.sealBlock(rhsBlock);

	// Code should now be generated in the RHS block
	ctx.mBlock = rhsBlock;
	Value* rhsVal = mRHS->emitIR(ctx);

	// This is the final RHS block (for the phi node)
	rhsBlock = ctx.mBlock;

	// Add the branch and the end of the RHS
	{
		IRBuilder<> build(ctx.mBlock);
		rhsVal = build.CreateICmpNE(rhsVal, ctx.mZero, "tobool");

		// We do an unconditional branch because the phi mode will handle
		// the correct value
		build.CreateBr(endBlock);
	}

	// endBlock should now be sealed
	ctx.mSSA.sealBlock(endBlock);

	ctx.mBlock = endBlock;

	IRBuilder<> build(ctx.mBlock);

	// Figure out the value to zext
	Value* zextVal = nullptr;

	// If rhs is not also true, we need to make a phi
	if (rhsVal != ConstantInt::getTrue(ctx.mGlobal))
	{
		PHINode* phi = build.CreatePHI(llvm::Type::getInt1Ty(ctx.mGlobal), 2);
		// If we came from the lhs, it had to be false
		phi->addIncoming(ConstantInt::getTrue(ctx.mGlobal), lhsBlock);
		phi->addIncoming(rhsVal, rhsBlock);
		zextVal = phi;
	}
	else
	{
		zextVal = ConstantInt::getTrue(ctx.mGlobal);
	}

	return build.CreateZExt(zextVal, llvm::Type::getInt32Ty(ctx.mGlobal));
}

AST_EMIT(ASTBinaryCmpOp)
{
	Value* retVal = nullptr;

	// PA3: Implement

	Value* lhsVal = mLHS->emitIR(ctx);
	Value* rhsVal = mRHS->emitIR(ctx);

	IRBuilder<> build(ctx.mBlock);

	switch(mOp){
		case uscc::scan::Token::EqualTo :
			retVal = build.CreateICmpEQ(lhsVal, rhsVal);
			break;
		case uscc::scan::Token::NotEqual :
			retVal = build.CreateICmpNE(lhsVal, rhsVal);
			break;
		case uscc::scan::Token::LessThan :
			retVal = build.CreateICmpSLT(lhsVal, rhsVal);
			break;
		case uscc::scan::Token::GreaterThan :
			retVal = build.CreateICmpSGT(lhsVal, rhsVal);
			break;
		default:
			break;
	}

	retVal = build.CreateZExt(retVal, llvm::Type::getInt32Ty(ctx.mGlobal));

	return retVal;
}

AST_EMIT(ASTBinaryMathOp)
{
	Value* retVal = nullptr;

	// PA3: Implement
	Value* lhsVal = mLHS->emitIR(ctx);
	Value* rhsVal = mRHS->emitIR(ctx);

	IRBuilder<> build(ctx.mBlock);

	switch(mOp){
		case uscc::scan::Token::Plus :
			retVal = build.CreateAdd(lhsVal, rhsVal);
			break;
		case uscc::scan::Token::Minus :
			retVal = build.CreateSub(lhsVal, rhsVal);
			break;
		case uscc::scan::Token::Mult :
			retVal = build.CreateMul(lhsVal, rhsVal);
			break;
		case uscc::scan::Token::Div :
			retVal = build.CreateSDiv(lhsVal, rhsVal);
			break;
		case uscc::scan::Token::Mod :
			retVal = build.CreateSRem(lhsVal, rhsVal);
			break;
		default:
			break;
	}

	return retVal;
}

// Value -->
AST_EMIT(ASTNotExpr)
{
	Value* retVal = nullptr;

	// PA3: Implement
	Value* val = mExpr->emitIR(ctx);
	llvm::Type* type = val->getType();

	IRBuilder<> build(ctx.mBlock);

	retVal = build.CreateICmpEQ(val, ConstantInt::get(type, 0));

	retVal = build.CreateZExt(retVal, llvm::Type::getInt32Ty(ctx.mGlobal));

	return retVal;
}

// Factor -->
AST_EMIT(ASTConstantExpr)
{
	Value* retVal = nullptr;

	// PA3: Implement
	llvm::Type* type = nullptr;
	if(mType == Type::Char){
		type = llvm::Type::getInt8Ty(ctx.mGlobal);
	}
	else{
		type = llvm::Type::getInt32Ty(ctx.mGlobal);
	}

	retVal = ConstantInt::get(type, mValue);

	return retVal;
}

AST_EMIT(ASTStringExpr)
{
	return mString->getValue();
}

AST_EMIT(ASTIdentExpr)
{
	return mIdent.readFrom(ctx);
}

AST_EMIT(ASTArrayExpr)
{
	// Generate the array subscript, which'll give us the address
	Value* addr = mArray->emitIR(ctx);

	IRBuilder<> build(ctx.mBlock);
	// Now load this value and return

	// NOTE: This still needs to be a load because arrays are in memory
	llvm::Type* elemTy = mArray->getIdent().llvmElementType(ctx.mGlobal);
	return build.CreateLoad(elemTy, addr);
}

AST_EMIT(ASTArrayInitExpr)
{
	for (auto elem : mElems)
	{
		elem->emitIR(ctx);
	}
	return nullptr;
}

AST_EMIT(ASTFuncExpr)
{
	// Create the list of arguments
	std::vector<Value*> callList;
	for (auto arg : mArgs)
	{
		callList.push_back(arg->emitIR(ctx));
	}

	// Now call the function, and return it
	Value* retVal = nullptr;

	IRBuilder<> build(ctx.mBlock);
	Function* callee = llvm::cast<Function>(mIdent.getAddress());
	if (mType != Type::Void)
	{
		retVal = build.CreateCall(callee, callList, "call");
	}
	else
	{
		retVal = build.CreateCall(callee, callList);
	}

	return retVal;
}

AST_EMIT(ASTIncExpr)
{
	Value* retVal = nullptr;

	// PA3: Implement
	Value* val = mIdent.readFrom(ctx);
	llvm::Type* type = val->getType();

	IRBuilder<> build(ctx.mBlock);
	
	retVal = build.CreateAdd(val, ConstantInt::get(type, 1));
	
	mIdent.writeTo(ctx, retVal);

	return retVal;
}

AST_EMIT(ASTDecExpr)
{
	Value* retVal = nullptr;

	// PA3: Implement
	Value* val = mIdent.readFrom(ctx);
	llvm::Type* type = val->getType();

	IRBuilder<> build(ctx.mBlock);
	
	retVal = build.CreateSub(val, ConstantInt::get(type, 1));
	
	mIdent.writeTo(ctx, retVal);

	return retVal;
}

AST_EMIT(ASTAddrOfArray)
{
	return mArray->emitIR(ctx);
}

AST_EMIT(ASTToIntExpr)
{
	Value* exprVal = mExpr->emitIR(ctx);
	IRBuilder<> build(ctx.mBlock);
	return build.CreateSExt(exprVal, llvm::Type::getInt32Ty(ctx.mGlobal), "conv");
}

AST_EMIT(ASTToCharExpr)
{
	Value* exprVal = mExpr->emitIR(ctx);
	IRBuilder<> build(ctx.mBlock);
	return build.CreateTrunc(exprVal, llvm::Type::getInt8Ty(ctx.mGlobal), "conv");
}

// Declaration
void ASTDecl::emitInitializerList(CodeContext& ctx,
                                  std::shared_ptr<ASTArrayInitExpr> init,
                                  size_t dimIdx, size_t base) noexcept
{
	// PA3: Implement (Array initializer)
	IRBuilder<> build(ctx.mBlock);

	// first call clears array
	if(dimIdx == 0){
		auto arrAddress = mIdent.readFrom(ctx);
		size_t byteCount = mIdent.getArrayCount();
		if(mIdent.getType() == Type::IntArray){
			byteCount *= 4;
		}
		build.CreateMemSet(arrAddress, ConstantInt::get(llvm::Type::getInt8Ty(ctx.mGlobal), 0), byteCount, llvm::MaybeAlign(1));
	}

	// 
	size_t counter = 0;
	for(const auto& elem : init->getElems()){
		auto nested = std::dynamic_pointer_cast<ASTArrayInitExpr>(elem);
		if(nested != nullptr){
			emitInitializerList(ctx, nested, dimIdx + 1, base + counter * mIdent.getStride(dimIdx));
		}
		else{
			Value* val = elem->emitIR(ctx);
			IRBuilder<> builder(ctx.mBlock);
			llvm::Type* type = mIdent.llvmElementType(ctx.mGlobal);

			// const - i32 ; char - i8
			if(val->getType() != type){
				val = builder.CreateTrunc(val, type);
			}

			Value* idx = ConstantInt::get(llvm::Type::getInt32Ty(ctx.mGlobal), base + counter);
			Value* addr = builder.CreateInBoundsGEP(type, mIdent.readFrom(ctx), idx);
			builder.CreateStore(val, addr);
		}
		counter++;
	}
}

AST_EMIT(ASTDecl)
{
	// If there's an expression, emit this also and store it in the ident
	if (mExpr)
	{
		auto initList = std::dynamic_pointer_cast<ASTArrayInitExpr>(mExpr);
		if (initList)
		{
			emitInitializerList(ctx, initList, 0, 0);
			return nullptr;
		}

		Value* declExpr = mExpr->emitIR(ctx);

		IRBuilder<> build(ctx.mBlock);
		// If this is a string, we have to memcpy
		if (declExpr->getType()->isPointerTy())
		{
			// This address should already be saved
			Value* arrayLoc = mIdent.readFrom(ctx);

			build.CreateMemCpy(arrayLoc, llvm::MaybeAlign(1),
							   declExpr,  llvm::MaybeAlign(1),
							   mIdent.getArrayCount());
		}
		else
		{
			// Basic types can just be written
			mIdent.writeTo(ctx, declExpr);
		}
	}

	return nullptr;
}

// Statements
AST_EMIT(ASTCompoundStmt)
{
	// PA3: Implement
	// acc to astnode.h mDecls => std::list<shared_ptr<ASTDecl>>
	for(std::shared_ptr<ASTDecl> decl : mDecls){
		decl->emitIR(ctx);
	}

	// mStmt => list of shared_prt<ASTStmt>
	for(std::shared_ptr<ASTStmt> stmt : mStmts){
		if(ctx.mBlock->getTerminator()){
			break;
		}
		stmt->emitIR(ctx);
	}

	return nullptr;
}

AST_EMIT(ASTAssignStmt)
{
	// This is simpler than decl because we don't allow
	// assignments to happen later for full arrays

	// PA3: Implement

	Value* val = mExpr->emitIR(ctx);
	mIdent.writeTo(ctx, val);

	return nullptr;
}

AST_EMIT(ASTAssignArrayStmt)
{
	// Generate the expression
	Value* exprVal = mExpr->emitIR(ctx);

	// Generate the array subscript, which'll give us the address
	Value* addr = mArray->emitIR(ctx);

	IRBuilder<> build(ctx.mBlock);

	// NOTE: This is still a create store because arrays are always stack-allocated
	build.CreateStore(exprVal, addr);

	return nullptr;
}

AST_EMIT(ASTIfStmt)
{
	// PA3: Implement
	BasicBlock* ifThen = BasicBlock::Create(ctx.mGlobal, "if.then", ctx.mFunc);	// if true then block
	BasicBlock* ifEnd = BasicBlock::Create(ctx.mGlobal, "if.end", ctx.mFunc);	// if stmt end block
	BasicBlock* ifElse = nullptr;	// else stmt block
	BasicBlock* falseTarget = ifEnd;	// if condition is false block

	if(mElseStmt != nullptr){
		ifElse = BasicBlock::Create(ctx.mGlobal, "if.else", ctx.mFunc);
		falseTarget = ifElse;
	}

	auto emittedCondition = mExpr->emitIR(ctx);
	IRBuilder<> conditionBuild(ctx.mBlock);

	auto conditionBool = conditionBuild.CreateICmpNE(emittedCondition, ctx.mZero, "tobool");
	
	auto conditionBranch = conditionBuild.CreateCondBr(conditionBool, ifThen, falseTarget);

	ctx.mBlock = ifThen;
	auto emittedThen = mThenStmt->emitIR(ctx);

	if(ctx.mBlock->getTerminator() == nullptr){
		IRBuilder<> thenBuild(ctx.mBlock);
		thenBuild.CreateBr(ifEnd);
	}

	if(mElseStmt != nullptr){
		ctx.mBlock = ifElse;
		auto emittedElse = mElseStmt->emitIR(ctx);
		if(ctx.mBlock->getTerminator() == nullptr){
			IRBuilder<> elseBuild(ctx.mBlock);
			elseBuild.CreateBr(ifEnd);
		}
	}

	ctx.mBlock = ifEnd;

	return nullptr;
}

AST_EMIT(ASTWhileStmt)
{
	// PA3: Implement
	BasicBlock* conditionBlock = BasicBlock::Create(ctx.mGlobal, "while.cond", ctx.mFunc);	// while condition
	BasicBlock* bodyBlock = BasicBlock::Create(ctx.mGlobal, "while.body", ctx.mFunc);	// loop body
	BasicBlock* endBlock = BasicBlock::Create(ctx.mGlobal, "while.end", ctx.mFunc);	// loop end

	IRBuilder<> preBuild(ctx.mBlock);
	preBuild.CreateBr(conditionBlock);

	ctx.mBlock = conditionBlock;
	auto emitted = mExpr->emitIR(ctx);

	IRBuilder<> condBuild(ctx.mBlock);

	auto converted = condBuild.CreateICmpNE(emitted, ctx.mZero, "tobool");

	auto conditionBranch = condBuild.CreateCondBr(converted, bodyBlock, endBlock);

	ctx.mBlock = bodyBlock;

	ctx.mBreakBlocks.push(endBlock);
	ctx.mContinueBlocks.push(conditionBlock);

	emitted = mLoopStmt->emitIR(ctx);

	ctx.mBreakBlocks.pop();
	ctx.mContinueBlocks.pop();

	if(ctx.mBlock->getTerminator() == nullptr){
		IRBuilder<> bodyBuild(ctx.mBlock);
		conditionBranch = bodyBuild.CreateBr(conditionBlock);
	}

	ctx.mBlock = endBlock;

	return nullptr;
}

AST_EMIT(ASTReturnStmt)
{
	// PA3: Implement
	Value* val = nullptr;
	if(mExpr != nullptr){
		val = mExpr->emitIR(ctx);
		IRBuilder<> build(ctx.mBlock);
		build.CreateRet(val);
	}
	else{
		IRBuilder<> build(ctx.mBlock);
		build.CreateRetVoid();
	}

	return nullptr;
}

AST_EMIT(ASTExprStmt)
{
	// PA3: Implement
	// Emit the expression, just return the value
    llvm::Value* retVal = nullptr;
	retVal = mExpr->emitIR(ctx);

	return retVal;
}

AST_EMIT(ASTNullStmt)
{
	// Doesn't do anything (hence empty)
	return nullptr;
}

AST_EMIT(ASTBreakStmt)
{
    // PA3: Implement
	IRBuilder<> build(ctx.mBlock);
	build.CreateBr(ctx.mBreakBlocks.top());

	return nullptr;
}

AST_EMIT(ASTContinueStmt)
{
    // PA3: Implement
	IRBuilder<> build(ctx.mBlock);
	build.CreateBr(ctx.mContinueBlocks.top());

	return nullptr;
}

AST_EMIT(ASTForStmt)
{
	// PA3: Implement
	BasicBlock* forCond = BasicBlock::Create(ctx.mGlobal, "for.cond", ctx.mFunc);
	BasicBlock* forBody = BasicBlock::Create(ctx.mGlobal, "for.body", ctx.mFunc);
	BasicBlock* forStep = BasicBlock::Create(ctx.mGlobal, "for.step", ctx.mFunc);
	BasicBlock* forEnd = BasicBlock::Create(ctx.mGlobal, "for.end", ctx.mFunc);

	if(minitStmt != nullptr){
		minitStmt->emitIR(ctx);
	}

	IRBuilder<> preBuild(ctx.mBlock);
	preBuild.CreateBr(forCond);

	ctx.mBlock = forCond;
	Value* condVal = nullptr;

	if(mcondExpr != nullptr){
		condVal = mcondExpr->emitIR(ctx);
	}

	IRBuilder<> condBuild(ctx.mBlock);
	if(condVal != nullptr){
		auto converted = condBuild.CreateICmpNE(condVal, ctx.mZero, "tobool");
		condBuild.CreateCondBr(converted, forBody, forEnd);
	}
	else{
		condBuild.CreateBr(forBody);
	}

	ctx.mBlock = forBody;
	
	ctx.mBreakBlocks.push(forEnd);
	ctx.mContinueBlocks.push(forStep);

	mbodyStmt->emitIR(ctx);

	ctx.mBreakBlocks.pop();
	ctx.mContinueBlocks.pop();

	if(ctx.mBlock->getTerminator() == nullptr){
		IRBuilder<> bodyBuild(ctx.mBlock);
		bodyBuild.CreateBr(forStep);
	}

	ctx.mBlock = forStep;
	if(mstepStmt != nullptr){
		mstepStmt->emitIR(ctx);
	}
	if(ctx.mBlock->getTerminator() == nullptr){
		IRBuilder<> stepBuild(ctx.mBlock);
		stepBuild.CreateBr(forCond);
	}

	ctx.mBlock = forEnd;

	return nullptr;
}

AST_EMIT(ASTDoWhileStmt)
{
	// PA3: Implement
	BasicBlock* conditionBlock = BasicBlock::Create(ctx.mGlobal, "dowhile.cond", ctx.mFunc);	// while condition
	BasicBlock* bodyBlock = BasicBlock::Create(ctx.mGlobal, "dowhile.body", ctx.mFunc);	// loop body
	BasicBlock* endBlock = BasicBlock::Create(ctx.mGlobal, "dowhile.end", ctx.mFunc);	// loop end

	IRBuilder<> preBuild(ctx.mBlock);
	preBuild.CreateBr(bodyBlock);

	ctx.mBlock = bodyBlock;

	ctx.mBreakBlocks.push(endBlock);
	ctx.mContinueBlocks.push(conditionBlock);

	mLoopStmt->emitIR(ctx);

	ctx.mBreakBlocks.pop();
	ctx.mContinueBlocks.pop();

	if(ctx.mBlock->getTerminator() == nullptr){
		IRBuilder<> bodyBuild(ctx.mBlock);
		bodyBuild.CreateBr(conditionBlock);
	}

	ctx.mBlock = conditionBlock;
	auto emitted = mExpr->emitIR(ctx);

	IRBuilder<> condBuild(ctx.mBlock);

	auto converted = condBuild.CreateICmpNE(emitted, ctx.mZero, "tobool");

	condBuild.CreateCondBr(converted, bodyBlock, endBlock);

	ctx.mBlock = endBlock;

	return nullptr;
}

AST_EMIT(ASTSwitchStmt)
{
	// PA3: Implement
	auto conditionVal = mExpr->emitIR(ctx);
	
	std::vector<BasicBlock*> blocks;
	BasicBlock* defaultBlock = nullptr;

	for(const auto& stmt: case_default_stmts){
		auto isDefault = std::dynamic_pointer_cast<ASTDefaultStmt>(stmt);
		if(isDefault != nullptr){
			defaultBlock = BasicBlock::Create(ctx.mGlobal, "switch.default", ctx.mFunc);
			blocks.push_back(defaultBlock);
		}
		else{
			BasicBlock* caseBlock = BasicBlock::Create(ctx.mGlobal, "switch.case", ctx.mFunc);
			blocks.push_back(caseBlock);
		}
	}

	BasicBlock* endBlock = BasicBlock::Create(ctx.mGlobal, "switch.end", ctx.mFunc);

	IRBuilder<> build(ctx.mBlock);
	BasicBlock* destination = nullptr;

	if(defaultBlock != nullptr){
		destination = defaultBlock;
	}
	else{
		destination = endBlock;
	}
	
	SwitchInst* sw = build.CreateSwitch(conditionVal, destination, case_default_stmts.size());

	for(size_t i = 0; i < case_default_stmts.size(); i++){
		auto caseStmt = std::dynamic_pointer_cast<ASTCaseStmt>(case_default_stmts[i]);

		if(caseStmt != nullptr){
			auto val = caseStmt->getExpr()->emitIR(ctx);
			sw->addCase(llvm::cast<ConstantInt>(val), blocks[i]);
		}
	}

	ctx.mBreakBlocks.push(endBlock);

	for(size_t i = 0; i < case_default_stmts.size(); i++){
		ctx.mBlock = blocks[i];
		case_default_stmts[i]->emitIR(ctx);

		if(ctx.mBlock->getTerminator() == nullptr){
			BasicBlock* next = endBlock;
			if((i + 1) < blocks.size()){
				next = blocks[i+1];
			}

			IRBuilder<> endBuild(ctx.mBlock);
			endBuild.CreateBr(next);
		}
	}

	ctx.mBreakBlocks.pop();
	ctx.mBlock = endBlock;

	return nullptr;
}

AST_EMIT(ASTCaseStmt)
{
    for (const auto& innerStmt : mStmts)
    {
        if (ctx.mBlock->getTerminator())
        {
            break;
        }
        innerStmt->emitIR(ctx);
    }
	return nullptr;
}

AST_EMIT(ASTDefaultStmt)
{
    for (const auto& innerStmt : mStmts)
    {
        if (ctx.mBlock->getTerminator())
        {
            break;
        }
        innerStmt->emitIR(ctx);
    }
	return nullptr;
}

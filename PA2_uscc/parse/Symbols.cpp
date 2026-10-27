//
//  Symbols.cpp
//  uscc
//
//  Implements the symbol and string tables used for
//  semantic analysis.
//
//---------------------------------------------------------
//  Copyright (c) 2014, Sanjay Madhav
//  All rights reserved.
//
//  This file is distributed under the BSD license.
//  See LICENSE.TXT for details.
//---------------------------------------------------------

#include "Symbols.h"
#include "Emitter.h"
#include "ASTNodes.h"
#include "Types.h"
#include <cstddef>
#include <iterator>
#include <memory>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#pragma clang diagnostic pop

#include <vector>
#include <algorithm>
#include <ostream>

using namespace uscc::parse;

llvm::Type* Identifier::llvmType(llvm::LLVMContext& context, bool treatArrayAsPtr /* = true */) noexcept
{
	llvm::Type* type = nullptr;
	switch (mType)
	{
		case Type::Char:
			type = llvm::Type::getInt8Ty(context);
			break;
		case Type::Int:
			type = llvm::Type::getInt32Ty(context);
			break;
		case Type::Void:
			type = llvm::Type::getVoidTy(context);
			break;
		case Type::CharArray:
			// Are we treating arrays as pointers?
			if (!treatArrayAsPtr)
			{
				type = llvm::ArrayType::get(llvm::Type::getInt8Ty(context),
											mArrayCount);
			}
			else
			{
				type = llvm::PointerType::getUnqual(context);
			}
			break;
		case Type::IntArray:
			// Are we treating arrays as pointers?
			if (!treatArrayAsPtr)
			{
				type = llvm::ArrayType::get(llvm::Type::getInt32Ty(context),
											mArrayCount);
			}
			else
			{
				type = llvm::PointerType::getUnqual(context);
			}
			break;
		case Type::Function:
			break;
	}

	return type;
}

llvm::Type* Identifier::llvmType(bool treatArrayAsPtr /* = true */) noexcept
{
	if (mAddress != nullptr)
	{
		return llvmType(mAddress->getContext(), treatArrayAsPtr);
	}
	return nullptr;
}

llvm::Type* Identifier::llvmElementType(llvm::LLVMContext& context) noexcept
{
	switch (mType)
	{
		case Type::Char:
		case Type::CharArray:
			return llvm::Type::getInt8Ty(context);
		case Type::Int:
		case Type::IntArray:
			return llvm::Type::getInt32Ty(context);
		default:
			return nullptr;
	}
}

llvm::Value* Identifier::readFrom(CodeContext& ctx) noexcept
{
	// PA4: Rewrite this entire function

	llvm::Value* retVal = nullptr;
	// Special case for arrays local to this function
	if (isArray() && getArrayCount() != -1)
	{
		retVal = getAddress();
	}
	else
	{
		// PA3: Load from the memory address of this identifier
	}
	return retVal;
}

void Identifier::writeTo(CodeContext& ctx, llvm::Value* value) noexcept
{
	// PA4: Rewrite this entire function

	// Special case for arrays local to this function
	if (isArray() && getArrayCount() != -1)
	{
		setAddress(value);
	}
	else
	{
		// PA3: Write to memory address of this identifier
	}
}

SymbolTable::SymbolTable() noexcept
: mCurrScope(nullptr)
{
	// PA2: Implement
	// enter current scope
	enterScope();

	// create 3 identifiers
	Identifier* funcIdent = createIdentifier("@@function");
	Identifier* varIdent = createIdentifier("@@variable");
	Identifier* printIdent = createIdentifier("printf");

	// set types
	funcIdent->setType(Type::Function);
	varIdent->setType(Type::Int);
	printIdent->setType(Type::Function);

	// not required after implementing createIdentifier(const char* name)
	// mCurrScope->addIdentifier(funcIdent);
	// mCurrScope->addIdentifier(varIdent);
	// mCurrScope->addIdentifier(printIdent);
}

SymbolTable::~SymbolTable() noexcept
{
	// PA2: Implement
	if(mCurrScope != nullptr){
		ScopeTable* localTable = mCurrScope;
		while(localTable->getParent() != nullptr){
			localTable = localTable->getParent();
		}
		delete localTable;
	}
}

// Returns true if this variable is already declared
// in this scope (ignoring parent scopes).
// Used to prevent redeclaration in the same scope,
// which is disallowed.
bool SymbolTable::isDeclaredInScope(const char* name) const noexcept
{
	// PA2: Implement
	if(mCurrScope != nullptr){
		if(mCurrScope->searchInScope(name) != nullptr){
			return true;
		}
	}
	return false;
}

// Creates the requested identifier, and returns a pointer
// to it.
// NOTE: If the identifier already exists, nothing will happen.
// This means you should first check with isDeclaredInScope.
Identifier* SymbolTable::createIdentifier(const char* name)
{
	Identifier* ident = new Identifier(name);

	// PA2: Implement
	mCurrScope->addIdentifier(ident);

	return ident;
}

// Returns a pointer to the identifier, if it's found
// Otherwise returns nullptr
Identifier* SymbolTable::getIdentifier(const char* name)
{
	// PA2: Implement properly
	if(mCurrScope != nullptr){
		Identifier* retVal = mCurrScope->search(name);
		if(retVal != nullptr){
			return retVal;
		}
	}

	return nullptr;
}

// Enters a new scope, and returns a pointer to this scope table
SymbolTable::ScopeTable* SymbolTable::enterScope()
{
	// PA2: Implement
	ScopeTable* newTable = new ScopeTable(mCurrScope);

	mCurrScope = newTable;

	return mCurrScope;

	return nullptr;
}

// Prints the symbol table to the specified stream
void SymbolTable::print(std::ostream& output) const noexcept
{
	output << "Symbols:\n";
	if (mCurrScope)
	{
		mCurrScope->print(output);
	}
}

// Exits the current scope and moves the current scope back to
// the previous scope table.
void SymbolTable::exitScope()
{
	// PA2: Implement
	if(mCurrScope != nullptr){
		mCurrScope = mCurrScope->getParent();
	}
}

SymbolTable::ScopeTable::ScopeTable(ScopeTable* parent) noexcept
: mParent(parent)
{
	// PA2: Implement
	if(parent != nullptr){
		parent->mChildren.push_back(this);
	}
}

SymbolTable::ScopeTable::~ScopeTable() noexcept
{
	// PA2: Implement
	for(auto& x : mSymbols){
		delete x.second;
	}

	for(auto x : mChildren){
		delete x;
	}
}

// Adds the requested identifier to the table
void SymbolTable::ScopeTable::addIdentifier(Identifier* ident)
{
	// PA2: Implement
	std::string identName = ident->getName();
	mSymbols.insert({identName, ident});
}

// Searches this scope for an identifier with
// the requested name. Returns nullptr if not found.
Identifier* SymbolTable::ScopeTable::searchInScope(const char* name) noexcept
{
	// PA2: Implement
	auto iterator = mSymbols.find(name);
	// for(auto it = mSymbols.begin(); it != mSymbols.end(); it++){
	// 	if(it == iterator){
	// 		return iterator->second;
	// 	}
	// }	no need loop, iterator gives O(1) look up
	if(iterator != mSymbols.end()){
		return iterator->second;
	}
	return nullptr;
}

// Searches this scope first, and if not found searches
// through parent scopes. Returns nullptr if not found.
Identifier* SymbolTable::ScopeTable::search(const char* name) noexcept
{
	// PA2: Implement
	ScopeTable* localTable = this;

	while(localTable != nullptr){
		Identifier* retVal = localTable->searchInScope(name);
		if(retVal != nullptr){
			return retVal;
		}
		else{
			localTable = localTable->getParent();
		}
	}
	return nullptr;
}

bool SymbolTable::checkShadow(const char* name)
{
	// PA2: Implement
	if(mCurrScope->getParent() != nullptr){
		ScopeTable* currParent = mCurrScope->getParent();
		while(currParent != nullptr){
			Identifier* searchRes = currParent->searchInScope(name);
			if(searchRes != nullptr){
				if(searchRes->getType() != Type::Function){
					return true;
				}
			}
			currParent = currParent->getParent();
		}
	}
	return false;
}

bool SymbolTable::checkArrayIdx(const char* name, std::shared_ptr<ASTExpr> expr, size_t dimIdx)
{
	// PA2: Implement
	std::shared_ptr<ASTConstantExpr> constCheck = std::dynamic_pointer_cast<ASTConstantExpr>(expr);
	if(constCheck == nullptr){
		return false;
	}

	Identifier* ident = getIdentifier(name);
	if(ident == nullptr || !ident->isArray()){
		return false;
	}

	size_t extent = ident->getExtent(dimIdx);
	int index = constCheck->getValue();
	if(index < 0 || (size_t)index >= extent){
		return true;
	}
    return false;
}

void SymbolTable::ScopeTable::emitIR(CodeContext& ctx)
{
	// The ONLY thing we should alloca now are arrays of a specified size
	// First emit all the symbols in this scope
	for (auto sym : mSymbols)
	{
		Identifier* ident = sym.second;
		llvm::IRBuilder<> build(ctx.mBlock);

		llvm::Value* decl = nullptr;

		std::string name = ident->getName();

		// It's -1 if it's an array that's passed into a function,
		// in which case we don't allocate it
		if (ident->isArray() && ident->getArrayCount() != -1)
		{
			llvm::Type* type = ident->llvmType(ctx.mGlobal, false);
			// Note we pass in "nullptr" for the array size because that's
			// handled by the type
			decl = build.CreateAlloca(type, nullptr, name);
			llvm::cast<llvm::AllocaInst>(decl)->setAlignment(llvm::Align(8));

			// Make a GEP here so we can access it later on without issue
			std::vector<llvm::Value*> gepIdx;
			gepIdx.push_back(ctx.mZero);
			gepIdx.push_back(ctx.mZero);

			decl = build.CreateInBoundsGEP(type, decl, gepIdx);

			// Now write this GEP and save it for this identifier
			ident->writeTo(ctx, decl);
		}
		else
		{
			// PA4: Remove this else case

			// PA3: Alloca the variables local to this function,
			// and save the address.
			// (Make sure you check for function arguments, which
			// will already have a value which we needs to be copied)

		}
	}

	// Now emit all the variables in the child scope tables
	for (auto table : mChildren)
	{
		table->emitIR(ctx);
	}
}

// Prints the scope table to the specified stream
void SymbolTable::ScopeTable::print(std::ostream& output, int depth) const noexcept
{
	std::vector<Identifier*> idents;
	for (const auto& sym : mSymbols)
	{
		idents.push_back(sym.second);
	}

	std::sort(idents.begin(), idents.end(), [](Identifier* a, Identifier* b) {
		return a->getName() < b->getName();
	});

	for (const auto& ident : idents)
	{
		if (ident->getName()[0] == '@')
		{
			continue;
		}

		for (int i = 0; i < depth; i++)
		{
			output << "---";
		}

		switch (ident->getType())
		{
		case Type::Void:
			output << "void ";
			break;
		case Type::Int:
			output << "int ";
			break;
		case Type::Char:
			output << "char ";
			break;
		case Type::IntArray:
			output << "int[] ";
			break;
		case Type::CharArray:
			output << "char[] ";
			break;
		case Type::Function:
			output << "function ";
			break;
		default:
			output << "unknown ";
			break;
		}

		output << ident->getName();
		output << '\n';
	}

	for (const auto& child : mChildren)
	{
		child->print(output, depth + 1);
	}
}

StringTable::StringTable() noexcept
{

}

StringTable::~StringTable() noexcept
{
	for (auto i : mStrings)
	{
		delete i.second;
	}
}

// Looks up the requested string in the string table
// If it exists, returns the corresponding ConstStr
// Otherwise, constructs a new ConstStr and returns that
ConstStr* StringTable::getString(std::string& val) noexcept
{
	auto iter = mStrings.find(val);
	if (iter != mStrings.end())
	{
		return iter->second;
	}
	else
	{
		ConstStr* newStr = new ConstStr(val);
		mStrings.emplace(val, newStr);
		return newStr;
	}
}

void StringTable::emitIR(CodeContext& ctx) noexcept
{
	for (auto s : mStrings)
	{
		ConstStr* str = s.second;
		// Make the llvm value for this string
		llvm::Constant* strVal = llvm::ConstantDataArray::getString(ctx.mGlobal, str->mText);

		// Add this to the global table
		llvm::ArrayType* type = llvm::ArrayType::get(llvm::Type::getInt8Ty(ctx.mGlobal),
													 str->mText.size() + 1);


		llvm::GlobalValue* globVal =
			new llvm::GlobalVariable(*ctx.mModule, type, true,
									 llvm::GlobalValue::LinkageTypes::PrivateLinkage,
									 strVal, ".str");
		// This can be "unnamed" since the address location is not significant
		globVal->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
		// Strings are 1-aligned
		//globVal->setAlignment(1);

		str->mValue = globVal;
	}
}

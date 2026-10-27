//
//  ParseExpr.cpp
//  uscc
//
//  Implements all of the recursive descent parsing
//  functions for the expression grammar rules.
//
//---------------------------------------------------------
//  Copyright (c) 2014, Sanjay Madhav
//  All rights reserved.
//
//  This file is distributed under the BSD license.
//  See LICENSE.TXT for details.
//---------------------------------------------------------

#include "ASTNodes.h"
#include "Parse.h"
#include "ParseExcept.h"
#include "Symbols.h"
#include <initializer_list>
#include <iostream>
#include <memory>
#include <sstream>

using namespace uscc::parse;
using namespace uscc::scan;

using std::shared_ptr;
using std::make_shared;

shared_ptr<ASTExpr> Parser::parseExpr()
{
	shared_ptr<ASTExpr> retVal;
	
	// We should first get a AndTerm
	shared_ptr<ASTExpr> andTerm = parseAndTerm();
	
	// If we didn't get an andTerm, then this isn't an Expr
	if (andTerm)
	{
		retVal = andTerm;
		// Check if this is followed by an op (optional)
		shared_ptr<ASTLogicalOr> exprPrime = parseExprPrime(retVal);
		
		if (exprPrime)
		{
			// If we got a exprPrime, return this instead of just term
			retVal = exprPrime;
		}
	}
	
	return retVal;
}

shared_ptr<ASTLogicalOr> Parser::parseExprPrime(shared_ptr<ASTExpr> lhs)
{
	shared_ptr<ASTLogicalOr> retVal;
	
	// Must be ||
	if (peekToken() == Token::Or)
	{
		// Make the binary cmp op
		Token::Tokens op = peekToken();
		retVal = make_shared<ASTLogicalOr>();
		consumeToken();
		
		// Set the lhs to our parameter
		retVal->setLHS(lhs);
		
		// We MUST get a AndTerm as the RHS of this operand
		shared_ptr<ASTExpr> rhs = parseAndTerm();
		if (!rhs)
		{
			throw OperandMissing(op);
		}
		
		retVal->setRHS(rhs);
		
		// PA2: Finalize op
		
		// See comment in parseTermPrime if you're confused by this
		shared_ptr<ASTLogicalOr> exprPrime = parseExprPrime(retVal);
		if (exprPrime)
		{
			retVal = exprPrime;
		}
	}
	
	return retVal;
}

// AndTerm -->
shared_ptr<ASTExpr> Parser::parseAndTerm()
{
	shared_ptr<ASTExpr> retVal;
	shared_ptr<ASTLogicalAnd> prime;

	// PA1: This should not directly check factor
	// but instead implement the proper grammar rule
	shared_ptr<ASTExpr> lhs = parseRelExpr();
	if(lhs != nullptr){
		shared_ptr<ASTLogicalAnd> nextNode = parseAndTermPrime(lhs);
		if(nextNode != nullptr){
			retVal = nextNode;
		}
		else{
			retVal = lhs;
		}
	}
	
	return retVal;
}

shared_ptr<ASTLogicalAnd> Parser::parseAndTermPrime(shared_ptr<ASTExpr> lhs)
{
	shared_ptr<ASTLogicalAnd> retVal;

	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	std::initializer_list<Token::Tokens> tokenList = {Token::And};
	if(!peekIsOneOf(tokenList)){
		retVal = nullptr;
	}
	else{
		consumeToken();
		shared_ptr<ASTLogicalAnd> newNode = make_shared<ASTLogicalAnd>();
		newNode->setLHS(lhs);
		
		shared_ptr<ASTExpr> rhs = parseRelExpr();
		if(rhs == nullptr){
			throw OperandMissing(Token::And);
		}
		newNode->setRHS(rhs);
		
		shared_ptr<ASTLogicalAnd> nextNewNode = parseAndTermPrime(newNode);
		if(nextNewNode != nullptr){
			retVal = nextNewNode;
		} 
		else{
			retVal = newNode;
		}
	}
	
	return retVal;
}

// RelExpr -->
shared_ptr<ASTExpr> Parser::parseRelExpr()
{
	shared_ptr<ASTExpr> retVal;

	// PA1: Implement
	shared_ptr<ASTExpr> lhs = parseNumExpr();
	if(lhs != nullptr){
		shared_ptr<ASTBinaryCmpOp> nextNode = parseRelExprPrime(lhs);
		if(nextNode != nullptr){
			retVal = nextNode;
		}
		else{
			retVal = lhs;
		}
	}
	
	return retVal;
}

shared_ptr<ASTBinaryCmpOp> Parser::parseRelExprPrime(shared_ptr<ASTExpr> lhs)
{
	shared_ptr<ASTBinaryCmpOp> retVal;
	
	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	std::initializer_list<Token::Tokens> tokenList = {Token::EqualTo, Token::LessThan, Token::GreaterThan, Token::NotEqual};
	if(!peekIsOneOf(tokenList)){
		retVal = nullptr;
	}
	else{
		consumeToken();
		shared_ptr<ASTBinaryCmpOp> newNode = make_shared<ASTBinaryCmpOp>(currentToken);
		newNode->setLHS(lhs);
		
		shared_ptr<ASTExpr> rhs = parseNumExpr();
		if(rhs == nullptr){
			throw OperandMissing(currentToken);
		}
		newNode->setRHS(rhs);
		
		shared_ptr<ASTBinaryCmpOp> nextNewNode = parseRelExprPrime(newNode);
		if(nextNewNode != nullptr){
			retVal = nextNewNode;
		} 
		else{
			retVal = newNode;
		}
	}
	
	return retVal;
}

// NumExpr -->
shared_ptr<ASTExpr> Parser::parseNumExpr()
{
	shared_ptr<ASTExpr> retVal;
	
	// PA1: Implement
	shared_ptr<ASTExpr> lhs = parseTerm();
	if(lhs != nullptr){
		shared_ptr<ASTBinaryMathOp> nextNode = parseNumExprPrime(lhs);
		if(nextNode != nullptr){
			retVal = nextNode;
		}
		else{
			retVal = lhs;
		}
	}

	return retVal;
}

shared_ptr<ASTBinaryMathOp> Parser::parseNumExprPrime(shared_ptr<ASTExpr> lhs)
{
	shared_ptr<ASTBinaryMathOp> retVal;

	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	std::initializer_list<Token::Tokens> tokenList = {Token::Plus, Token::Minus};
	if(!peekIsOneOf(tokenList)){
		retVal = nullptr;
	}
	else{
		consumeToken();
		shared_ptr<ASTBinaryMathOp> newNode = make_shared<ASTBinaryMathOp>(currentToken);
		newNode->setLHS(lhs);
		
		shared_ptr<ASTExpr> rhs = parseTerm();
		if(rhs == nullptr){
			throw OperandMissing(currentToken);
		}
		newNode->setRHS(rhs);
		
		shared_ptr<ASTBinaryMathOp> nextNewNode = parseNumExprPrime(newNode);
		if(nextNewNode != nullptr){
			retVal = nextNewNode;
		} 
		else{
			retVal = newNode;
		}
	}
	
	return retVal;
}

// Term -->
shared_ptr<ASTExpr> Parser::parseTerm()
{
	shared_ptr<ASTExpr> retVal;

	// PA1: Implement
	retVal = parseValue();
	if(retVal != nullptr){
		shared_ptr<ASTBinaryMathOp> nextNode = parseTermPrime(retVal);
		if(nextNode != nullptr){
			retVal = nextNode;
		}
	}

	return retVal;
}

shared_ptr<ASTBinaryMathOp> Parser::parseTermPrime(shared_ptr<ASTExpr> lhs)
{
	shared_ptr<ASTBinaryMathOp> retVal;

	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	std::initializer_list<Token::Tokens> tokenList = {Token::Mult, Token::Mod, Token::Div};
	if(!peekIsOneOf(tokenList)){
		retVal = nullptr;
	}
	else{
		consumeToken();
		shared_ptr<ASTBinaryMathOp> newNode = make_shared<ASTBinaryMathOp>(currentToken);
		newNode->setLHS(lhs);
		//parseValue();
		shared_ptr<ASTExpr> rhs = parseValue();
		if(rhs == nullptr){
			throw OperandMissing(currentToken);
		}
		newNode->setRHS(rhs);
		
		shared_ptr<ASTBinaryMathOp> nextNewNode = parseTermPrime(newNode);
		if(nextNewNode != nullptr){
			retVal = nextNewNode;
		} 
		else{
			retVal = newNode;
		}
	}
	
	return retVal;
}

// Value -->
shared_ptr<ASTExpr> Parser::parseValue()
{
	shared_ptr<ASTExpr> retVal;
	
	// PA1: Implement
	Token::Tokens currentToken = peekToken();

	if(currentToken == Token::Not){
		consumeToken();
		shared_ptr<ASTExpr> nextToken = parseFactor();
		if(nextToken == nullptr){
			throw ParseExceptMsg("! must be followed by an expression.");
		}
		else{
			shared_ptr<ASTNotExpr> newNode = make_shared<ASTNotExpr>(nextToken);
			retVal = newNode;
		}
	}
	else{
		retVal = parseFactor();
	}

	return retVal;
}

// Factor -->
shared_ptr<ASTExpr> Parser::parseFactor()
{
	shared_ptr<ASTExpr> retVal;
	
	// Try parse identifier factors FIRST so
	// we make sure to consume the mUnusedIdents
	// before we try any other rules
	
	if ((retVal = parseIdentFactor()))
		;
	// PA1: Add additional cases
	else if((retVal = parseConstantFactor()))
		;

	else if((retVal = parseStringFactor()))
		;

	else if((retVal = parseParenFactor()))
		;

	else if((retVal = parseIncFactor()))
		;

	else if((retVal = parseDecFactor())) 
		;
	else if((retVal = parseAddrOfArrayFactor()))
		;
	return retVal;
}

// ( Expr )
shared_ptr<ASTExpr> Parser::parseParenFactor()
{
	shared_ptr<ASTExpr> retVal;

	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	if(currentToken != Token::LParen){
		return nullptr;
	}
	else{
		peekAndConsume(currentToken);
		retVal = parseExpr();
		if(retVal == nullptr){
			throw ParseExceptMsg("Not a valid expression inside parenthesis");
		}
		else{
			matchToken(Token::RParen);
		}
	}
	
	return retVal;
}

// constant
shared_ptr<ASTConstantExpr> Parser::parseConstantFactor()
{
	shared_ptr<ASTConstantExpr> retVal = nullptr;
	
	// PA1: Implement
	bool isConst = peekToken() == Token::Constant ? true : false;
	if(isConst){	// check constant
		std::string tokenText = getTokenTxt();	// store
		consumeToken();
		retVal = make_shared<ASTConstantExpr>(tokenText);	// create ast node
	}

	return retVal;
}

// ConstantExpr --> ConstantTerm ((+|-) ConstantTerm)*
shared_ptr<ASTConstantExpr> Parser::parseConstantExpr()
{
	shared_ptr<ASTConstantExpr> retVal;

	// PA1: Implement
	shared_ptr<ASTConstantExpr> leftToken = parseConstantTerm();
	std::initializer_list<Token::Tokens> tokenList = {Token::Plus, Token::Minus};	// tokens to consider
	while(peekIsOneOf(tokenList)){	// next token should be one of the above
		Token::Tokens currentToken = peekToken();
		peekAndConsume(currentToken);
		shared_ptr<ASTConstantExpr> rightToken = parseConstantTerm();

		int leftVal = leftToken->getValue();	// get values of the token
		int rightVal = rightToken->getValue();
		int calcVal;

		// perform calculation
		if(currentToken == Token::Plus){
			calcVal = leftVal + rightVal;
		}
		else if(currentToken == Token::Minus){
			calcVal = leftVal - rightVal;
		}

		// create ast node
		shared_ptr<ASTConstantExpr> newASTnode = make_shared<ASTConstantExpr>(calcVal);
		leftToken = newASTnode;	// assign new node to left token so loop can continue
	}

	retVal = leftToken;

	return retVal;
}

// ConstantTerm --> ConstantOperand ((*|/|%) ConstantOperand)*
shared_ptr<ASTConstantExpr> Parser::parseConstantTerm()
{
	shared_ptr<ASTConstantExpr> retVal;

	// PA1: Implement
	shared_ptr<ASTConstantExpr> leftToken = parseConstantOperand();
	std::initializer_list<Token::Tokens> tokenList = {Token::Mult, Token::Mod, Token::Div};	// token list
	while(peekIsOneOf(tokenList)){	// token must be one of the above
		Token::Tokens currentToken = peekToken();
		peekAndConsume(currentToken);
		shared_ptr<ASTConstantExpr> rightToken = parseConstantOperand();
		int leftVal = leftToken->getValue();
		int rightVal = rightToken->getValue();
		int calcVal;

		// calculate value
		if(currentToken == Token::Mult){
			calcVal = leftVal * rightVal;
		}
		else if(currentToken == Token::Mod){
			calcVal = leftVal % rightVal;
		}
		else if(currentToken == Token::Div){
			calcVal = leftVal / rightVal;
		}

		shared_ptr<ASTConstantExpr> newASTnode = make_shared<ASTConstantExpr>(calcVal);	// create ast node
		leftToken = newASTnode;	// assign to left token to continue loop
	}

	retVal = leftToken;

	return retVal;
}

// ConstantOperand --> constant | ( ConstantExpr )
shared_ptr<ASTConstantExpr> Parser::parseConstantOperand()
{
	shared_ptr<ASTConstantExpr> retVal;

	// PA1: Implement
	retVal = parseConstantFactor();
	if(retVal != nullptr){
		return retVal;
	}
	else{
		if(peekToken() == Token::LParen){
			peekAndConsume(Token::LParen);
			retVal = parseConstantExpr();
			matchToken(Token::RParen);
		}
	}

	return retVal;
}

// string
shared_ptr<ASTStringExpr> Parser::parseStringFactor()
{
	shared_ptr<ASTStringExpr> retVal = nullptr;

	// PA1: Implement
	bool isStr = peekToken() == Token::String ? true : false;
	if(isStr){	// check str
		std::string tokenText = getTokenTxt();	//store
		consumeToken();
		retVal = make_shared<ASTStringExpr>(tokenText, mStrings);	// ast node for str required token text and string table
	}

	return retVal;
}

// id
// id [ Expr ]
// id ( FuncCallArgs )
shared_ptr<ASTExpr> Parser::parseIdentFactor()
{
	shared_ptr<ASTExpr> retVal;
	if (peekToken() == Token::Identifier ||
		mUnusedIdent != nullptr || mUnusedArray != nullptr)
	{
		if (mUnusedArray)
		{
			// "unused array" means that AssignStmt looked at this array
			// and decided it didn't want it, so it's already made an
			// array sub node
			retVal = make_shared<ASTArrayExpr>(mUnusedArray);
			mUnusedArray = nullptr;
		}
		else
		{
			Identifier* ident = nullptr;
			
			// If we have an "unused identifier," which means that
			// AssignStmt looked at this and decided it didn't want it,
			// that means we're already a token AFTER the identifier.
			if (mUnusedIdent)
			{
				ident = mUnusedIdent;
				mUnusedIdent = nullptr;
			}
			else
			{
				ident = getVariable(getTokenTxt());
				consumeToken();
			}
			
			// Now we need to look ahead and see if this is an array
			// or function call reference, since id is a common
			// left prefix.
			if (peekToken() == Token::LBracket)
			{
				// Check to make sure this is an array
				if (mCheckSemant && ident->getType() != Type::IntArray &&
					ident->getType() != Type::CharArray &&
					!ident->isDummy())
				{
					std::string err("'");
					err += ident->getName();
					err += "' is not an array";
					reportSemantError(err);
					consumeUntil(Token::RBracket);
					if (peekToken() == Token::EndOfFile)
					{
						throw EOFExcept();
					}
					
					matchToken(Token::RBracket);
					
					// Just return our error variable
					retVal = make_shared<ASTIdentExpr>(*mSymbols.getIdentifier("@@variable"));
				}
				else
				{
					consumeToken();
					shared_ptr<ASTArraySub> array;
					try
					{
						shared_ptr<ASTExpr> expr = parseExpr();
						if (!expr)
						{
							throw ParseExceptMsg("Valid expression required inside [ ].");
						}

						array = make_shared<ASTArraySub>(*ident, expr);
						retVal = make_shared<ASTArrayExpr>(array);
					}
					catch (ParseExcept& e)
					{
						// If this expr is bad, recover to the closing bracket
						reportError(e);
						synchronize({Token::RBracket, Token::SemiColon, Token::RBrace}, false);
					}

					matchToken(Token::RBracket);

					while (array && peekAndConsume(Token::LBracket))
					{
						shared_ptr<ASTExpr> expr = parseExpr();
						if (expr)
						{
							array->addExpr(expr);
						}
						matchToken(Token::RBracket);
					}
				}
			}
			else if (peekToken() == Token::LParen)
			{
				// Check to make sure this is a function
				if (mCheckSemant && ident->getType() != Type::Function &&
					!ident->isDummy())
				{
					std::string err("'");
					err += ident->getName();
					err += "' is not a function";
					reportSemantError(err);
					consumeUntil(Token::RParen);
					if (peekToken() == Token::EndOfFile)
					{
						throw EOFExcept();
					}
					
					matchToken(Token::RParen);
					
					// Just return our error variable
					retVal = make_shared<ASTIdentExpr>(*mSymbols.getIdentifier("@@variable"));
				}
				else
				{
					consumeToken();
					// A function call can have zero or more arguments
					shared_ptr<ASTFuncExpr> funcCall = make_shared<ASTFuncExpr>(*ident);
					retVal = funcCall;
					
					// Get the number of arguments for this function
					shared_ptr<ASTFunction> func = ident->getFunction();
					
					try
					{
						int currArg = 1;
						int col = mColNumber;
						shared_ptr<ASTExpr> arg = parseExpr();
						while (arg)
						{
							// Check for validity of this argument (for non-dummy functions)
							if (!ident->isDummy())
							{
								// Special case for "printf" since we don't make a node for it
								if (ident->getName() == "printf")
								{
									mNeedPrintf = true;
									if (currArg == 1 && arg->getType() != Type::CharArray)
									{
										reportSemantError("The first parameter to printf must be a char[]");
									}
								}
								else if (mCheckSemant)
								{
									if (currArg > func->getNumArgs())
									{
										std::string err("Function ");
										err += ident->getName();
										err += " takes only ";
										std::ostringstream ss;
										ss << func->getNumArgs();
										err += ss.str();
										err += " arguments";
										reportSemantError(err, col);
									}
									else if (!func->checkArgType(currArg, arg->getType()))
									{
										// If we have an int and the expected arg type is a char,
										// we can do a conversion
										if (arg->getType() == Type::Int &&
											func->getArgType(currArg) == Type::Char)
										{
											arg = intToChar(arg);
										}
										else
										{
											std::string err("Expected expression of type ");
											err += getTypeText(func->getArgType(currArg));
											reportSemantError(err, col);
										}
									}
								}
							}
							
							funcCall->addArg(arg);
							
							currArg++;
							
							if (peekAndConsume(Token::Comma))
							{
								col = mColNumber;
								arg = parseExpr();
								if (!arg)
								{
									throw
									ParseExceptMsg("Comma must be followed by expression in function call");
								}
							}
							else
							{
								break;
							}
						}
					}
					catch (ParseExcept& e)
					{
						reportError(e);
						synchronize({Token::RParen, Token::SemiColon, Token::RBrace}, false);
					}
					
					// Now make sure we have the correct number of arguments
					if (!ident->isDummy())
					{
						// Special case for printf
						if (ident->getName() == "printf")
						{
							if (funcCall->getNumArgs() == 0)
							{
								reportSemantError("printf requires a minimum of one argument");
							}
						}
						else if (mCheckSemant && funcCall->getNumArgs() < func->getNumArgs())
						{
							std::string err("Function ");
							err += ident->getName();
							err += " requires ";
							std::ostringstream ss;
							ss << func->getNumArgs();
							err += ss.str();
							err += " arguments";
							reportSemantError(err);
						}
					}
					
					matchToken(Token::RParen);
				}
			}
			else
			{
				// Just a plain old ident
				retVal = make_shared<ASTIdentExpr>(*ident);
			}
		}
	}
	
	return retVal;
}

// ++ id
shared_ptr<ASTExpr> Parser::parseIncFactor()
{
	shared_ptr<ASTExpr> retVal;
	
	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	if(currentToken == Token::Inc){
		peekAndConsume(currentToken);

		currentToken = peekToken();
		if(currentToken == Token::Identifier){
			Identifier* identifier = getVariable(getTokenTxt());
			consumeToken();	
			shared_ptr<ASTIncExpr> newNode = make_shared<ASTIncExpr>(*identifier);
			retVal = newNode;
		}
		else{
			retVal = nullptr;
		}
	
	}
	else{
		retVal = nullptr;
	}
	
	return retVal;
}

// -- id
shared_ptr<ASTExpr> Parser::parseDecFactor()
{
	shared_ptr<ASTExpr> retVal;
	
	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	if(currentToken == Token::Dec){
		peekAndConsume(currentToken);

		currentToken = peekToken();
		if(currentToken == Token::Identifier){
			Identifier* identifier = getVariable(getTokenTxt());
			consumeToken();
			shared_ptr<ASTDecExpr> newNode = make_shared<ASTDecExpr>(*identifier);
			retVal = newNode;
		}
		else{
			retVal = nullptr;
		}
	}
	else{
		retVal = nullptr; 
	}
	return retVal;
}

// & id [ Expr ]
shared_ptr<ASTExpr> Parser::parseAddrOfArrayFactor()
{
	shared_ptr<ASTExpr> retVal;
	
	// PA1: Implement
	shared_ptr<ASTArraySub> arraySub;
	Token::Tokens currentToken = peekToken();
	if(currentToken == Token::Addr){
		consumeToken();
		if(peekToken() == Token::Identifier){
			Identifier* indentifier = getVariable(getTokenTxt());
			consumeToken();

			if(!peekAndConsume(Token::LBracket)){
				throw ParseExceptMsg("Missing required subscript expression.");
			}
			
			shared_ptr<ASTExpr> expr = parseExpr();
			if(expr != nullptr){
				arraySub = make_shared<ASTArraySub>(*indentifier, expr);
			}
			else{
				throw ParseExceptMsg("Missing required subscript expression.");
			}
			matchToken(Token::RBracket);

			while(peekAndConsume(Token::LBracket)){
				shared_ptr<ASTExpr> next = parseExpr();
				if(next != nullptr){
					arraySub->addExpr(next);
				}
				matchToken(Token::RBracket);
			}
			shared_ptr<ASTExpr> newNode = make_shared<ASTAddrOfArray>(arraySub);
			retVal = newNode;
		
		}
		else{
			throw ParseExceptMsg("& must be followed by an identifier.");
		}
	}
	else{
		retVal = nullptr;
	}

	return retVal;
}

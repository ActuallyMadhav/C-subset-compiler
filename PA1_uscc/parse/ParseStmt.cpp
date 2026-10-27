//
//  ParseStmt.cpp
//  uscc
//
//  Implements all of the recursive descent parsing
//  functions for statement grammar rules.
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
#include <memory>
#include <regex>

using namespace uscc::parse;
using namespace uscc::scan;

using std::shared_ptr;
using std::make_shared;

void Parser::synchronize(const std::initializer_list<Token::Tokens>& syncSet,
						 bool consumeTerminator)
{
	// PA1: Implement
	
	while (peekToken() != Token::EndOfFile)
	{
		Token::Tokens currentToken = peekToken();
		bool synced = false;
		for(Token::Tokens token : syncSet){
			if(currentToken == token){
				synced = true;
				break;
			}
		}

		if(synced || isStmtFirst(currentToken)){
			if(consumeTerminator && currentToken == Token::SemiColon){
				consumeToken(false);
			}

			return;
		}
		
		consumeToken(false);
	}
	throw EOFExcept();
}

bool Parser::isStmtFirst(Token::Tokens tok) const noexcept
{
	switch (tok)
	{
		case Token::Key_if:
		case Token::Key_while:
		case Token::Key_for:
		case Token::Key_do:
		case Token::Key_switch:
		case Token::Key_return:
		case Token::Key_break:
		case Token::Key_continue:
		case Token::LBrace:
			return true;
		default:
			return false;
	}
}

shared_ptr<ASTDecl> Parser::parseDecl()
{
	shared_ptr<ASTDecl> retVal;
	// A decl MUST start with int or char
	if (peekIsOneOf({Token::Key_int, Token::Key_char}))
	{
		Type baseType = Type::Void;
		if (peekToken() == Token::Key_int)
		{
			baseType = Type::Int;
		}
		else
		{
			baseType = Type::Char;
		}

		consumeToken();

		// Set this to @@variable for now. We'll later change it
		// assuming we parse the identifier properly
		Identifier* ident = mSymbols.getIdentifier("@@variable");

		// Now we MUST get an identifier so go into a try
		try
		{
			if (peekToken() != Token::Identifier)
			{
				throw ParseExceptMsg("Type must be followed by identifier");
			}

			ident = mSymbols.createIdentifier(getTokenTxt());

			consumeToken();

			retVal = finishDeclarator(*ident, baseType);

			matchToken(Token::SemiColon);
		}
		catch (ParseExcept& e)
		{
			reportError(e);

			// Recover to the next terminator or block close
			synchronize({Token::SemiColon, Token::RBrace}, true);

			// Put in a decl here with the bogus identifier
			// "@@error". This is so the parse will continue to the
			// next decl, if there is one.
			retVal = make_shared<ASTDecl>(*(ident));
		}
	}

	return retVal;
}

shared_ptr<ASTDecl> Parser::finishDeclarator(Identifier& ident, Type baseType)
{
	Type declType = baseType;

	// Is this an array declaration?
	if (peekToken() == Token::LBracket)
	{
		if (declType == Type::Int)
		{
			declType = Type::IntArray;
		}
		else
		{
			declType = Type::CharArray;
		}

		std::vector<int> dims;
		while (peekAndConsume(Token::LBracket))
		{
			shared_ptr<ASTConstantExpr> constExpr = parseConstantExpr();
			if (constExpr)
			{
				int count = constExpr->getValue();
				if (count <= 0 || count > 65536)
				{
					reportSemantError("Arrays must have a min of 1 and a max of 65536 elements");
				}
				dims.push_back(count);
			}
			else
			{
				// Int arrays must have a constant size defined; char
				// arrays may have an implicit size from an assignment
				if (declType == Type::IntArray)
				{
					reportSemantError("Int arrays must have a defined constant size");
				}
				dims.push_back(0);
			}

			matchToken(Token::RBracket);
		}

		size_t total = 1;
		for (int dim : dims)
		{
			total *= static_cast<size_t>(dim);
		}
		ident.setArrayCount(total);
		ident.setDims(dims);
	}

	ident.setType(declType);

	shared_ptr<ASTExpr> assignExpr;

	// Optionally, this decl may have an assignment
	if (peekAndConsume(Token::Assign))
	{
		if (peekToken() == Token::LBrace)
		{
			shared_ptr<ASTArrayInitExpr> initList = parseInitializerList();
			if (initList && ident.getArrayCount() == 0)
			{
				ident.setArrayCount(initList->getNumElems());
			}
			assignExpr = initList;
		}
		else
		{
			// We don't allow assignment for int arrays
			if (declType == Type::IntArray)
			{
				reportSemantError("USC does not allow assignment of int array declarations");
			}

			assignExpr = parseExpr();
			if (!assignExpr)
			{
				throw ParseExceptMsg("Invalid expression after = in declaration");
			}

			// PA2: Type checks

			// If this is a character array, we need to do extra checks
			if (ident.getType() == Type::CharArray)
			{
				ASTStringExpr* strExpr = dynamic_cast<ASTStringExpr*>(assignExpr.get());
				if (strExpr != nullptr)
				{
					// If we have a declared size, we need to make sure
					// there's enough room to fit the requested string.
					// Otherwise, we need to set our size
					if (ident.getArrayCount() == 0)
					{
						ident.setArrayCount(strExpr->getLength() + 1);
					}
					else if (ident.getArrayCount() < (strExpr->getLength() + 1))
					{
						reportSemantError("Declared array cannot fit string");
					}
				}
			}
		}
	}
	else if (ident.getType() == Type::CharArray && ident.getArrayCount() == 0)
	{
		reportSemantError("char array must have declared size if there's no assignment");
	}

	return make_shared<ASTDecl>(ident, assignExpr);
}

shared_ptr<ASTArrayInitExpr> Parser::parseInitializerList()
{
	shared_ptr<ASTArrayInitExpr> retVal;

	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	if(currentToken == Token::LBrace){
		consumeToken();
		retVal = make_shared<ASTArrayInitExpr>();

		Token::Tokens next = peekToken();
		if(next != Token::RBrace){
			do{
				shared_ptr<ASTExpr> listItem;

				if(peekToken() == Token::LBrace){
					listItem = parseInitializerList();
				}
				else{
					listItem = parseExpr();
				}
				retVal->addElem(listItem);
			}while(peekAndConsume(Token::Comma));

			matchToken(Token::RBrace);
		}
		return retVal;
	}

	return retVal;
}

shared_ptr<ASTStmt> Parser::parseStmt()
{
	shared_ptr<ASTStmt> retVal;
	try
	{
		// NOTE: AssignStmt HAS to go before ExprStmt!!
		// Read comments in AssignStmt for why.
		if ((retVal = parseCompoundStmt()))
			;
		else if ((retVal = parseAssignStmt()))
			;
		// PA1: Add additional cases
		else if((retVal = parseReturnStmt()))
			;
		else if((retVal = parseWhileStmt()))
			;
		else if((retVal = parseExprStmt()))
			;
		else if((retVal = parseNullStmt()))
			;
		else if((retVal = parseIfStmt()))
			;
		else if((retVal = parseBreakStmt()))
			;
		else if((retVal = parseContinueStmt()))
			;
		else if((retVal = parseForStmt()))
			;
		else if((retVal = parseDoWhileStmt()))
			;
		else if((retVal = parseSwitchStmt()))
			;
		else if((retVal = parseCaseStmt()))
			;
		else if (peekIsOneOf({Token::Key_int, Token::Key_char}))
		{
			throw ParseExceptMsg("Declarations are only allowed at the beginning of a scope block");
		}

	}
	catch (ParseExcept& e)
	{
		reportError(e);

		// Recover to the next terminator, block close, or statement start
		synchronize({Token::SemiColon, Token::RBrace}, true);

		// Put in a null statement here
		// so we can try to continue.
		retVal = make_shared<ASTNullStmt>();
	}

	return retVal;
}

// If the compound statement is a function body, then the symbol table scope
// change will happen at a higher level, so it shouldn't happen in
// parseCompoundStmt.
shared_ptr<ASTCompoundStmt> Parser::parseCompoundStmt(bool isFuncBody)
{
	shared_ptr<ASTCompoundStmt> retVal = nullptr;

	// PA1: Implement
	bool isLBrace = peekAndConsume(Token::LBrace);	// check first token is '{'
	if(isLBrace){
		retVal = make_shared<ASTCompoundStmt>();	// initialise retVal

		shared_ptr<ASTDecl> decl = parseDecl();
		while(decl != nullptr){	// parse declarations until nullptr
			retVal->addDecl(decl);
			decl = parseDecl();
		}

		shared_ptr<ASTStmt> stmt = parseStmt();
		while(stmt != nullptr){	// parse statements until nullptr
			retVal->addStmt(stmt);
			stmt = parseStmt();
		}

		matchToken(Token::RBrace);	// check last token is '}'
	}
	else{
		return retVal;
	}

	return retVal;
}


shared_ptr<ASTStmt> Parser::parseAssignStmt(bool isForStep)
{
	shared_ptr<ASTStmt> retVal;
	shared_ptr<ASTArraySub> arraySub;

	if (peekToken() == Token::Identifier)
	{
		Identifier* ident = getVariable(getTokenTxt());

		consumeToken();

		// Now let's see if this is an array subscript
		if (peekAndConsume(Token::LBracket))
		{
			try
			{
				shared_ptr<ASTExpr> expr = parseExpr();
				if (!expr)
				{
					throw ParseExceptMsg("Valid expression required inside [ ].");
				}

				arraySub = make_shared<ASTArraySub>(*ident, expr);
			}
			catch (ParseExcept& e)
			{
				// If this expr is bad, recover to the closing bracket
				reportError(e);
				synchronize({Token::RBracket, Token::SemiColon, Token::RBrace}, false);
			}

			matchToken(Token::RBracket);

			while (arraySub && peekAndConsume(Token::LBracket))
			{
				shared_ptr<ASTExpr> expr = parseExpr();
				if (expr)
				{
					arraySub->addExpr(expr);
				}
				matchToken(Token::RBracket);
			}
		}

		// Just because we got an identifier DOES NOT necessarily mean
		// this is an assign statement.
		// This is because there is a common left prefix between
		// AssignStmt and an ExprStmt with statements like:
		// id ;
		// id [ Expr ] ;
		// id ( FuncCallArgs ) ;

		// So... We see if the next token is a =. If it is, then this is
		// an AssignStmt. Otherwise, we set the "unused" variables
		// so parseFactor will later find it and be able to match
		int col = mColNumber;
		shared_ptr<ASTExpr> expr;

	    if (peekIsOneOf({Token::Assign, Token::PlusEqual, Token::MinusEqual, Token::MultiEqual, Token::DivEqual}))
        {
            shared_ptr<ASTExpr> expr;
            scan::Token::Tokens nextOp;
            bool ordinaryAssign = false;
            if (peekAndConsume(Token::Assign))
            {
                ordinaryAssign = true;
            }
            else if (peekIsOneOf({Token::PlusEqual, Token::MinusEqual, Token::MultiEqual, Token::DivEqual}))
            {
                scan::Token::Tokens opToken = peekToken();
                switch (opToken) {
                    case Token::PlusEqual:
                        nextOp = Token::Plus;
                        break;
                    case Token::MinusEqual:
                        nextOp = Token::Minus;
                        break;
                    case Token::MultiEqual:
                        nextOp = Token::Mult;
                        break;
                    case Token::DivEqual:
                        nextOp = Token::Div;
                        break;
                }
                consumeToken();
            }
            expr = parseExpr();

            if (!expr)
            {
                throw ParseExceptMsg("= must be followed by an expression");
            }

            // If we matched an array, we want to make an array assign stmt
            if (arraySub)
            {
                // Make sure the type of this expression matches the declared type
                Type subType;
                if (arraySub->getType() == Type::IntArray)
                {
                    subType = Type::Int;
                }
                else
                {
                    subType = Type::Char;
                }

                if (mCheckSemant && subType != expr->getType())
                {
                    // We can do a conversion if it's from int to char
                    if (subType == Type::Char &&
                        expr->getType() == Type::Int)
                    {
                        expr = intToChar(expr);
                    }
                    else
                    {
                        std::string err("Cannot assign an expression of type ");
                        err += getTypeText(expr->getType());
                        err += " to ";
                        err += getTypeText(subType);
                        reportSemantError(err, col);
                    }
                }

                if (ordinaryAssign) {
                    retVal = make_shared<ASTAssignArrayStmt>(arraySub, expr);
                }
                else {
                    shared_ptr<ASTBinaryMathOp> mathExpr = make_shared<ASTBinaryMathOp>(nextOp);
                    mathExpr->setLHS(std::dynamic_pointer_cast<ASTExpr>(arraySub));
                    mathExpr->setRHS(expr);
                    retVal = make_shared<ASTAssignArrayStmt>(arraySub, mathExpr);
                }
            }
            else
            {
                // PA2: Check for semantic errors

                if (ordinaryAssign) {
                    retVal = make_shared<ASTAssignStmt>(*ident, expr);
                }
                else {
                    shared_ptr<ASTBinaryMathOp> mathExpr = make_shared<ASTBinaryMathOp>(nextOp);
                    mathExpr->setLHS(make_shared<ASTIdentExpr>(*ident));
                    mathExpr->setRHS(expr);
                    retVal = make_shared<ASTAssignStmt>(*ident, mathExpr);
                }
            }

            if (!isForStep) matchToken(Token::SemiColon);
        }
		else
		{
			// We either have an unused array, or an unused ident
			if (arraySub)
			{
				mUnusedArray = arraySub;
			}
			else
			{
				mUnusedIdent = ident;
			}
		}
	}

	return retVal;
}

shared_ptr<ASTIfStmt> Parser::parseIfStmt()
{
	shared_ptr<ASTIfStmt> retVal;

	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	if(currentToken == Token::Key_if){
		consumeToken();
		matchToken(Token::LParen);

		shared_ptr<ASTExpr> ifCondition = parseExpr();
		if(ifCondition == nullptr){
			throw ParseExceptMsg("Invalid condition for if statement");
		}

		matchToken(Token::RParen);
		shared_ptr<ASTStmt> ifBody = parseStmt();
		shared_ptr<ASTStmt> elseBody = nullptr;
	
		if(peekToken() == Token::Key_else){
			consumeToken();
			elseBody = parseStmt();
		}
		shared_ptr<ASTIfStmt> newNode = make_shared<ASTIfStmt>(ifCondition, ifBody, elseBody);
		retVal = newNode;
	
	}
	else{
		retVal = nullptr;
	}

	return retVal;
}

shared_ptr<ASTWhileStmt> Parser::parseWhileStmt()
{
	shared_ptr<ASTWhileStmt> retVal;

	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	if(currentToken == Token::Key_while){
		consumeToken();

		matchToken(Token::LParen);
		shared_ptr<ASTExpr> loopCondition = parseExpr();
		if(loopCondition == nullptr){
			throw ParseExceptMsg("Invalid condition for while statement");
		}
		matchToken(Token::RParen);

		shared_ptr<ASTStmt> loopStmt = parseStmt();
		shared_ptr<ASTWhileStmt> whileNode = make_shared<ASTWhileStmt>(loopCondition, loopStmt);
		retVal = whileNode;
	}
	else{
		retVal = nullptr;
	}

	return retVal;
}

shared_ptr<ASTReturnStmt> Parser::parseReturnStmt()
{
	shared_ptr<ASTReturnStmt> retVal = nullptr;

	// PA1: Implement
	bool isReturn = peekAndConsume(Token::Key_return);
	if(isReturn){
		shared_ptr<ASTExpr> expr = parseExpr();
		retVal = make_shared<ASTReturnStmt>(expr);


		matchToken(Token::SemiColon);	// check last token is ';'
	}

	return retVal;
}

shared_ptr<ASTExprStmt> Parser::parseExprStmt()
{
	shared_ptr<ASTExprStmt> retVal;

	// PA1: Implement
	shared_ptr<ASTExpr> expr = parseExpr();
	if(expr != nullptr){
		matchToken(Token::SemiColon);
		shared_ptr<ASTExprStmt> newNode = make_shared<ASTExprStmt>(expr);
		retVal = newNode;
	}
	else{
		retVal = nullptr;
	}

	return retVal;
}

shared_ptr<ASTNullStmt> Parser::parseNullStmt()
{
	shared_ptr<ASTNullStmt> retVal;

	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	if(currentToken == Token::SemiColon){
		peekAndConsume(currentToken);
		shared_ptr<ASTNullStmt> newNode = make_shared<ASTNullStmt>();
		retVal = newNode;
	}
	else{
		//consumeToken();
		retVal = nullptr;
	}

	return retVal;
}

shared_ptr<ASTBreakStmt> Parser::parseBreakStmt()
{
	shared_ptr<ASTBreakStmt> retVal;

	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	if(currentToken == Token::Key_break){
		consumeToken();
		matchToken(Token::SemiColon);
		retVal = make_shared<ASTBreakStmt>();
	}
	else{
		retVal = nullptr;
	}

	return retVal;
}

shared_ptr<ASTContinueStmt> Parser::parseContinueStmt()
{
	shared_ptr<ASTContinueStmt> retVal;

	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	if(currentToken == Token::Key_continue){
		consumeToken();
		matchToken(Token::SemiColon);
		retVal = make_shared<ASTContinueStmt>();
	}
	else{
		retVal = nullptr;
	}

	return retVal;
}

shared_ptr<ASTForStmt> Parser::parseForStmt()
{
    // for (INIT; COND; STEP) {
    //    BODY
    // }
	shared_ptr<ASTForStmt> retVal;

	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	if(currentToken == Token::Key_for){
		consumeToken();
		matchToken(Token::LParen);

		shared_ptr<ASTStmt> loopInit = parseAssignStmt(false);
		if(loopInit == nullptr) matchToken(Token::SemiColon);

		shared_ptr<ASTExpr> loopCondition = parseExpr();
		matchToken(Token::SemiColon);

		shared_ptr<ASTStmt> loopStep = parseAssignStmt(true);
		if(loopStep == nullptr){
			shared_ptr<ASTExpr> loopStepExpr = parseExpr();
			if(loopStepExpr != nullptr){
				loopStep = make_shared<ASTExprStmt>(loopStepExpr);
			}
		}
		matchToken(Token::RParen);

		shared_ptr<ASTStmt> loopBody = parseStmt();

		shared_ptr<ASTStmt>loopConditionStmt = make_shared<ASTExprStmt>(loopCondition); 

		retVal = make_shared<ASTForStmt>(loopInit, loopConditionStmt, loopStep, loopBody);
		
	}

    return retVal;
}

shared_ptr<ASTDoWhileStmt> Parser::parseDoWhileStmt()
{
    // do {
    //  STMT
    // } while (expr)
	shared_ptr<ASTDoWhileStmt> retVal;

	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	if(currentToken == Token::Key_do){
		consumeToken();

		shared_ptr<ASTStmt> loopBody = parseStmt();
		matchToken(Token::Key_while);
		matchToken(Token::LParen);

		shared_ptr<ASTExpr> loopCondition = parseExpr();
		if(loopCondition == nullptr){
			throw ParseExceptMsg("Invalid condition for while statement");
		}
		matchToken(Token::RParen);
		matchToken(Token::SemiColon);

		shared_ptr<ASTDoWhileStmt> doWhileNode = make_shared<ASTDoWhileStmt>(loopCondition, loopBody);
		retVal = doWhileNode;
	}
	else{
		retVal = nullptr;
	}

    return retVal;
}

shared_ptr<ASTSwitchStmt> Parser::parseSwitchStmt()
{
    // switch ( Expr ) {
    //   case 1:
    //      BODY_1
    //   case 2:
    //      BODY_2
    //   ....
    //   default:
    // }
	shared_ptr<ASTSwitchStmt> retVal;

	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	if(currentToken == Token::Key_switch){
		consumeToken();
		matchToken(Token::LParen);

		shared_ptr<ASTExpr> switchExpr = parseExpr();

		matchToken(Token::RParen);
		matchToken(Token::LBrace);

		shared_ptr<ASTSwitchStmt> switchNode = make_shared<ASTSwitchStmt>(switchExpr);
		retVal = switchNode;

		shared_ptr<ASTCaseDefaultStmt> caseNode = parseCaseStmt();
		while(caseNode != nullptr){
			retVal->addStmt(caseNode);
			caseNode = parseCaseStmt();
		}
		matchToken(Token::RBrace);
	}


    return retVal;
}

shared_ptr<ASTCaseDefaultStmt> Parser::parseCaseStmt()
{
	shared_ptr<ASTCaseDefaultStmt> retVal;

	// PA1: Implement
	Token::Tokens currentToken = peekToken();
	if(currentToken == Token::Key_case){
		consumeToken();
		shared_ptr<ASTExpr> caseExpr = parseConstantExpr();
		matchToken(Token::Colon);
		shared_ptr<ASTCaseStmt> caseStmt = make_shared<ASTCaseStmt>(caseExpr);

		while(peekToken() != Token::Key_case && peekToken() != Token::Key_default && peekToken() != Token::RBrace){
			if (peekToken() == Token::Key_break){
				caseStmt->setBreak();
			}

			shared_ptr<ASTStmt> stmt = parseStmt();

			if (stmt == nullptr){
				break;
			}
			caseStmt->addStmt(stmt);
		}

		retVal = caseStmt;
	}
	else if(currentToken == Token::Key_default){
		consumeToken();
		matchToken(Token::Colon);
		shared_ptr<ASTDefaultStmt> defaultStmt = make_shared<ASTDefaultStmt>();

		while (peekToken() != Token::Key_case && peekToken() != Token::Key_default && peekToken() != Token::RBrace){

			shared_ptr<ASTStmt> stmt = parseStmt();

			if (stmt == nullptr){
				break;
			}
			defaultStmt->addStmt(stmt);
		}

		retVal = defaultStmt;
	}
	else{
		retVal = nullptr;
	}

    return retVal;
}

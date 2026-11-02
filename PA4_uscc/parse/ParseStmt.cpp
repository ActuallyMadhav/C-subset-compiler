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

#include "Parse.h"
#include "Symbols.h"

using namespace uscc::parse;
using namespace uscc::scan;

using std::shared_ptr;
using std::make_shared;

void Parser::synchronize(const std::initializer_list<Token::Tokens>& syncSet,
						 bool consumeTerminator)
{
	while (!peekIsOneOf(syncSet) && !isStmtFirst(peekToken()) &&
		   peekToken() != Token::EndOfFile)
	{
		consumeToken(false);
	}

	if (peekToken() == Token::EndOfFile)
	{
		throw EOFExcept();
	}

	if (consumeTerminator && peekToken() == Token::SemiColon)
	{
		consumeToken();
	}
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


			auto token = getTokenTxt();
			if (mSymbols.isDeclaredInScope(token))
				reportSemantError(std::string("Invalid redeclaration of identifier '") + std::string(token) + "'");

            if (mSymbols.checkShadow(token))
				reportSemantWarn(std::string("warning: declaration of '") + std::string(token) + std::string("' shadows a previous declaration"));

			ident = mSymbols.createIdentifier(getTokenTxt());

			consumeToken();

			retVal = finishDeclarator(*ident, baseType);

			matchToken(Token::SemiColon);
		}
		catch (ParseExcept& e)
		{
			reportError(e);

			// Recover to the next terminator, block close, or statement start
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
	bool missingBound = false;
	std::vector<int> dims;

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
					missingBound = true;
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

	int col = mColNumber;

	// Optionally, this decl may have an assignment
	if (peekAndConsume(Token::Assign))
	{
		if (peekToken() == Token::LBrace)
		{
			shared_ptr<ASTArrayInitExpr> initList = parseInitializerList();
			if (ident.getArrayCount() == 0)
			{
				ident.setArrayCount(initList->getNumElems());
				if (dims.size() == 1)
				{
					dims[0] = static_cast<int>(initList->getNumElems());
					ident.setDims(dims);
					missingBound = false;
				}
			}
			checkInitializerList(ident, initList, 0);
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

			if (ident.getType() == Type::Char)
			{
				if (assignExpr->getType() == Type::Int)
					assignExpr = intToChar(assignExpr);
			}
			if (ident.getType() != assignExpr->getType())
			{
				std::string err("Cannot assign an expression of type ");
				err += getTypeText(assignExpr->getType());
				err += " to ";
				err += getTypeText(ident.getType());
				reportSemantError(err, col);
			}

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

	if (missingBound)
	{
		reportSemantError("Int arrays must have a defined constant size");
	}

	return make_shared<ASTDecl>(ident, assignExpr);
}

bool Parser::checkInitializerList(Identifier& ident,
								  shared_ptr<ASTArrayInitExpr> init,
								  size_t dimIdx)
{
	if (!init)
	{
		return true;
	}

	if (!ident.isArray())
	{
		std::string err("Initializer list is not allowed for non-array '");
		err += ident.getName();
		err += "'";
		reportSemantError(err);
		return false;
	}

	const std::vector<int>& dims = ident.getDims();
	if (dimIdx >= dims.size())
	{
		std::string err("Nested initializer is too deep for array '");
		err += ident.getName();
		err += "'";
		reportSemantError(err);
		return false;
	}

	const std::list<shared_ptr<ASTExpr>>& elems = init->getElems();

	size_t nestedCount = 0;
	for (auto elem : elems)
	{
		if (std::dynamic_pointer_cast<ASTArrayInitExpr>(elem))
		{
			nestedCount++;
		}
	}

	if (nestedCount > 0 && nestedCount < elems.size())
	{
		std::string err("Cannot mix braced and unbraced initializers for array '");
		err += ident.getName();
		err += "'";
		reportSemantError(err);
		return false;
	}

	if (nestedCount > 0)
	{
		if (elems.size() > ident.getExtent(dimIdx))
		{
			std::string err("Too many initializers for array '");
			err += ident.getName();
			err += "' of size ";
			err += std::to_string(ident.getExtent(dimIdx));
			reportSemantError(err);
			return false;
		}

		for (auto elem : elems)
		{
			if (!checkInitializerList(ident,
									  std::dynamic_pointer_cast<ASTArrayInitExpr>(elem),
									  dimIdx + 1))
			{
				return false;
			}
		}

		return true;
	}

	size_t capacity = ident.getExtent(dimIdx);
	for (size_t i = dimIdx + 1; i < dims.size(); i++)
	{
		capacity *= ident.getExtent(i);
	}

	if (elems.size() > capacity)
	{
		std::string err("Too many initializers for array '");
		err += ident.getName();
		err += "' of size ";
		err += std::to_string(capacity);
		reportSemantError(err);
		return false;
	}

	Type elemType = (ident.getType() == Type::IntArray) ? Type::Int : Type::Char;
	for (auto elem : elems)
	{
		Type given = elem->getType();
		if (given != elemType && !(given == Type::Int && elemType == Type::Char) &&
			!(given == Type::Char && elemType == Type::Int))
		{
			std::string err("Cannot assign an expression of type ");
			err += getTypeText(given);
			err += " to ";
			err += getTypeText(elemType);
			reportSemantError(err);
			return false;
		}
	}

	return true;
}

void Parser::checkSubscript(Identifier& ident, shared_ptr<ASTExpr> expr, size_t dimIdx)
{
	if (!expr)
	{
		return;
	}

	int folded = 0;
	shared_ptr<ASTExpr> probe = expr;
	if (foldConstant(expr, folded))
	{
		probe = make_shared<ASTConstantExpr>(folded);
	}

	if (mSymbols.checkArrayIdx(ident.getName().c_str(), probe, dimIdx))
	{
		shared_ptr<ASTConstantExpr> constExpr = std::dynamic_pointer_cast<ASTConstantExpr>(probe);
		reportSemantWarn(std::string("warning: index ") + std::to_string(constExpr->getValue()) +
				std::string(" is outside bounds of array '") + ident.getName() +
				std::string("' of size ") + std::to_string(ident.getExtent(dimIdx)));
	}
}

shared_ptr<ASTArrayInitExpr> Parser::parseInitializerList()
{
	shared_ptr<ASTArrayInitExpr> retVal = make_shared<ASTArrayInitExpr>();

	matchToken(Token::LBrace);

	if (peekToken() != Token::RBrace)
	{
		do
		{
			shared_ptr<ASTExpr> elem;
			if (peekToken() == Token::LBrace)
			{
				elem = parseInitializerList();
			}
			else
			{
				elem = parseExpr();
			}

			if (elem)
			{
				retVal->addElem(elem);
			}
		}
		while (peekAndConsume(Token::Comma));
	}

	matchToken(Token::RBrace);

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
		else if ((retVal = parseReturnStmt()))
			;
		else if ((retVal = parseWhileStmt()))
			;
		else if ((retVal = parseExprStmt()))
			;
		else if ((retVal = parseNullStmt()))
			;
		else if ((retVal = parseIfStmt()))
				;
		else if ((retVal = parseForStmt()))
			;
		else if ((retVal = parseDoWhileStmt()))
			;
		else if ((retVal = parseSwitchStmt()))
			;
		else if ((retVal = parseBreakStmt()))
			;
		else if ((retVal = parseContinueStmt()))
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
	shared_ptr<ASTCompoundStmt> retVal;

	if (peekAndConsume(Token::LBrace))
	{
		if (!isFuncBody)
			mSymbols.enterScope();

		retVal = make_shared<ASTCompoundStmt>();
		shared_ptr<ASTDecl> decl;
		decl = parseDecl();
		while (decl != nullptr)
		{
			retVal->addDecl(decl);
			decl = parseDecl();
		}

		shared_ptr<ASTStmt> stmt, lastStmt; // preserve the last statment for check
		stmt = parseStmt();
		while (stmt != nullptr)
		{
			retVal->addStmt(stmt);
			lastStmt = stmt;
			stmt = parseStmt();
		}

		if (!std::dynamic_pointer_cast<ASTReturnStmt>(lastStmt) && isFuncBody)
		{
			if (mCurrReturnType == Type::Void)
				retVal->addStmt(make_shared<ASTReturnStmt>(nullptr));
			else
				reportSemantError("USC requires non-void functions to end with a return");
		}

		matchToken(Token::RBrace);

		if (!isFuncBody)
			mSymbols.exitScope();
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

		// PA2 semantic check: array subscript on non-array
        if (ident->getName() != "@@variable") {
            if (peekToken() == Token::LBracket && (ident->getType() != Type::IntArray && ident->getType() != Type::CharArray)) {
              std::string err(ident->getName());
              err += " is not an array";
              reportSemantError(err, mColNumber);
            }
        }

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

                checkSubscript(*ident, expr, 0);
			}
			catch (ParseExcept& e)
			{
				// If this expr is bad, recover to the closing bracket
				reportError(e);
				synchronize({Token::RBracket, Token::SemiColon, Token::RBrace}, false);
			}

			matchToken(Token::RBracket);

			size_t dimIdx = 1;
			while (arraySub && peekAndConsume(Token::LBracket))
			{
				shared_ptr<ASTExpr> expr = parseExpr();
				if (expr)
				{
					arraySub->addExpr(expr);
					checkSubscript(*ident, expr, dimIdx);
				}
				dimIdx++;
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
                if (ident->getType() == Type::Char)
                {
                    if (expr->getType() == Type::Int)
                        expr = intToChar(expr);
                }

                // after conversion if still not match
                if (ident->getType() != expr->getType())
                {
                    std::string err("Cannot assign an expression of type ");
                    err += getTypeText(expr->getType());
                    err += " to ";
                    err += getTypeText(ident->getType());
                    reportSemantError(err, col);
                }

                if (ident->getType() == Type::IntArray || ident->getType() == Type::CharArray)
                    reportSemantError("Reassignment of arrays is not allowed", col);

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

	if (peekAndConsume(Token::Key_if))
	{
		matchToken(Token::LParen);
		auto expr = parseExpr();
		if (!expr)
			throw ParseExceptMsg("Invalid condition for if statement");
		matchToken(Token::RParen);

		auto stmt = parseStmt();
		shared_ptr<ASTStmt> elseStmt;
		if (peekAndConsume(Token::Key_else))
			elseStmt = parseStmt();
		retVal = make_shared<ASTIfStmt>(expr, stmt, elseStmt);
	}

	return retVal;
}

shared_ptr<ASTWhileStmt> Parser::parseWhileStmt()
{
	shared_ptr<ASTWhileStmt> retVal;

	if (peekAndConsume(Token::Key_while))
	{
		shared_ptr<ASTExpr> expr;
		shared_ptr<ASTStmt> stmt;
		matchToken(Token::LParen);
		expr = parseExpr();
		if (!expr)
			throw ParseExceptMsg("Invalid condition for while statement");
		matchToken(Token::RParen);

		stmt = parseStmt();
		retVal = make_shared<ASTWhileStmt>(expr, stmt);
	}

	return retVal;
}

shared_ptr<ASTReturnStmt> Parser::parseReturnStmt()
{
	shared_ptr<ASTReturnStmt> retVal;

	if (peekAndConsume(Token::Key_return))
	{
		if (peekIsOneOf({Token::SemiColon}))
		{
			retVal = make_shared<ASTReturnStmt>(nullptr);

			if (mCurrReturnType != Type::Void)
				reportSemantError("Invalid empty return in non-void function");
			consumeToken();
		}
		else
		{
			int col = mColNumber;
			auto expr = parseExpr();

			if (mCurrReturnType == Type::Char)
			{
				if (expr->getType() == Type::Int)
					expr = intToChar(expr);
			}

			// after conversion if still not match
			if (mCurrReturnType != expr->getType())
			{
				std::string err("Expected type ");
				err += getTypeText(mCurrReturnType);
				err += " in return statement";
				reportSemantError(err, col);
			}

			retVal = make_shared<ASTReturnStmt>(expr);
			matchToken(Token::SemiColon);
		}
	}

	return retVal;
}

shared_ptr<ASTExprStmt> Parser::parseExprStmt()
{
	shared_ptr<ASTExprStmt> retVal;

	auto e = parseExpr();

    if (auto function_call = std::dynamic_pointer_cast<ASTFuncExpr>(e))
    {
        const std::string& fname = function_call->getIdent().getName();
        Identifier* id = mSymbols.getIdentifier(fname.c_str());
        if (id && id->isFunction())
        {
            auto fn = id->getFunction();
            if (fn && fn->getReturnType() != Type::Void)
            {
                reportSemantWarn("warning: ignoring return value of function '" + fname + "'");
            }
        }
    }

	if (e)
	{
		retVal = make_shared<ASTExprStmt>(e);
		matchToken(Token::SemiColon);
	}

	return retVal;
}

shared_ptr<ASTNullStmt> Parser::parseNullStmt()
{
	shared_ptr<ASTNullStmt> retVal;

	if (peekAndConsume(Token::SemiColon))
		retVal = make_shared<ASTNullStmt>();

	return retVal;
}

shared_ptr<ASTBreakStmt> Parser::parseBreakStmt()
{
	shared_ptr<ASTBreakStmt> retVal;

	if (peekAndConsume(Token::Key_break)) {
        matchToken(Token::SemiColon);
		retVal = make_shared<ASTBreakStmt>();
    }
	return retVal;
}

shared_ptr<ASTContinueStmt> Parser::parseContinueStmt()
{
	shared_ptr<ASTContinueStmt> retVal;

	if (peekAndConsume(Token::Key_continue)) {
        matchToken(Token::SemiColon);
		retVal = make_shared<ASTContinueStmt>();
    }
	return retVal;
}

shared_ptr<ASTForStmt> Parser::parseForStmt()
{
	shared_ptr<ASTForStmt> retVal;
    // for (INIT; COND; STEP) {
    //    BODY
    // }
    if (peekAndConsume(Token::Key_for))
    {
        matchToken(Token::LParen);
        // INIT: assignment or expression (no ';' here)
        auto initStmt = parseStmt();
        // COND:
        auto condExprStmt = parseStmt();

        // STEP:
	    shared_ptr<ASTStmt> stepStmt = nullptr;
	    shared_ptr<ASTExpr> stepExpr = nullptr;
        if ((stepStmt = parseAssignStmt(true)))
            ;
        else if ((stepExpr = parseIncFactor()) || (stepExpr = parseDecFactor())) {
            stepStmt = make_shared<ASTExprStmt>(stepExpr);
        }
	    matchToken(Token::RParen);

        // BODY:
        shared_ptr<ASTStmt> bodyStmt = parseStmt();
        retVal = make_shared<ASTForStmt>(initStmt, condExprStmt, stepStmt, bodyStmt);
    }
    return retVal;
}

shared_ptr<ASTDoWhileStmt> Parser::parseDoWhileStmt()
{
	shared_ptr<ASTDoWhileStmt> retVal;

    // do {
    //  STMT
    // } while (expr)
    shared_ptr<ASTStmt> stmt;
    shared_ptr<ASTExpr> expr;
	if (peekAndConsume(Token::Key_do))
	{
        // STMT
		stmt = parseStmt();
        matchToken(Token::Key_while);
		matchToken(Token::LParen);
		expr = parseExpr();
		matchToken(Token::RParen);
		matchToken(Token::SemiColon);
        retVal = make_shared<ASTDoWhileStmt>(expr, stmt);
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
	shared_ptr<ASTCaseDefaultStmt> case_default_stmt;
    shared_ptr<ASTExpr> expr;
	if (peekAndConsume(Token::Key_switch))
	{
		matchToken(Token::LParen);
		expr = parseExpr(); // constant expr
		matchToken(Token::RParen);
		matchToken(Token::LBrace);
        retVal = make_shared<ASTSwitchStmt>(expr);
        while (!peekAndConsume(Token::RBrace)) {
		    case_default_stmt = parseCaseStmt();
            retVal->addStmt(case_default_stmt);
        }
    }
    return retVal;
}

shared_ptr<ASTCaseDefaultStmt> Parser::parseCaseStmt()
{
	shared_ptr<ASTCaseDefaultStmt> retVal;
	shared_ptr<ASTStmt> stmt;
	shared_ptr<ASTExpr> expr;
    if (peekAndConsume(Token::Key_case))
    {
        expr = parseExpr();
        matchToken(Token::Colon);
        // Due to lack of Rbrace or LBrace, it won't go to compoundStmt.
        retVal = make_shared<ASTCaseStmt>(expr);
        while (peekToken() != Token::Key_case && peekToken() != Token::Key_default) {
            stmt = parseStmt();
            retVal->addStmt(stmt);
            if (peekToken() == Token::RBrace) {
                break;
            }
        }
    }
    else if (peekAndConsume(Token::Key_default))
    {
        retVal = make_shared<ASTDefaultStmt>();
        matchToken(Token::Colon);
        while (peekToken() != Token::RBrace) {
            stmt = parseStmt();
            retVal->addStmt(stmt);
        }
    }
    return retVal;
}

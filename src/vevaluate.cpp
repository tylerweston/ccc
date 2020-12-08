#include "vevaluate.hpp"
#include "nodes.hpp"
#include "common.hpp"
#include "symtable.hpp"
#include <memory>
#include <string>

void EvaluateVisitor::visit(VariableNode* n) 
{
	// grab this symbol from the symbol table
	SymbolTableEntry* symbolTableEntry;
	symbolTableEntry = this->symbolTable->GetSymbol(n->name);
	// make sure this symbol had been declared already
	if (symbolTableEntry == nullptr)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Undeclared variable: " << n->name <<"\n";
		exit(1);
	}
	// if it is, this expression evaluates to the type of this symbol
	n->evaluatedType = symbolTableEntry->Type;
	// Variables are not constant! (We don't support const)
	n->isConstant = false;
}

void EvaluateVisitor::visit(DeclarationNode* n) 
{
	// make sure type isn't void
	if (n->t == TypeName::tVoid)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Can only declare functions void.\n";
		exit(1);
	}
	// try to add this symbol to our symbol table 
	if (!this->symbolTable->AddSymbol(n->name, n->t, n->location))
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): ";
		std::cout << "Variable " << n->name << " already declared in this scope.\n";
		exit(1);
	}
}

void EvaluateVisitor::visit(DeclAndAssignNode* n) 
{
	// evaluate rhs expression
	n->expr->accept(this);
	// make sure type of rhs matches declared type
	if (n->decl->t != n->expr->evaluatedType)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Type mismatch between declaration and expression.\n";
		std::cout << "Expected type " << TypeNameString(n->decl->t);
		std::cout << " but got type " << TypeNameString(n->expr->evaluatedType) << "\n";
		exit(1);
	}
	// make sure type isn't void
	if (n->decl->t == TypeName::tVoid)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Can only declare functions void.\n";
		exit(1);
	}
	// Try to add this symbol to our symbol table
	if (!this->symbolTable->AddSymbol(n->decl->name, n->decl->t, n->decl->location))
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): ";
		std::cout << "Variable " << n->decl->name << " already declared in this scope.\n";
		exit(1);
	}
}

void EvaluateVisitor::visit(BinaryOpNode* n) 
{
	// evaluate our subexpressions
	n->left->accept(this);
	n->right->accept(this);
	// check that type of the children match
	if (n->left->evaluatedType != n->right->evaluatedType)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Type mismatch in binary operation\n";
		std::cout << "Got types " << TypeNameString(n->left->evaluatedType);
		std::cout << " and " << TypeNameString(n->right->evaluatedType) << "\n";
		exit(1);
	}
	// TODO: Some operations are only supported for tFloat and tInt
	// TODO: result of a LOG_AND or LOG_OR will always be int!

	// If we're here, our type matches our children
	n->evaluatedType = n->left->evaluatedType;
	// if both operands are constant, it's good
	n->isConstant = (n->left->isConstant && n->right->isConstant);
}

void EvaluateVisitor::visit(LogicalOpNode* n) 
{
	// evaluate our subexpressions
	n->left->accept(this);
	n->right->accept(this);
	// check that type of the children match
	if (n->left->evaluatedType != n->right->evaluatedType)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Type mismatch in logical operation\n";
		std::cout << "Got types " << TypeNameString(n->left->evaluatedType);
		std::cout << " and " << TypeNameString(n->right->evaluatedType) << "\n";
		exit(1);
	}
	// Type of a logical op node is always bool
	n->evaluatedType = TypeName::tBool;
	// if both operands are constant, it's good
	n->isConstant = (n->left->isConstant && n->right->isConstant);
}

void EvaluateVisitor::visit(RelationalOpNode* n) 
{
	// evaluate subexpressions
	n->left->accept(this);
	n->right->accept(this);
	// make sure type of children match
	if (n->left->evaluatedType != n->right->evaluatedType)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Type mismatch in relational operation \n";
		std::cout << "Got types " << TypeNameString(n->left->evaluatedType);
		std::cout << " and " << TypeNameString(n->right->evaluatedType) << "\n";
		exit(1);
	}
	// RelationalOps always evaluate to bool
	n->evaluatedType = TypeName::tBool;
	// if both of our operands are constant, we're constant
	n->isConstant = (n->left->isConstant && n->right->isConstant);
}

void EvaluateVisitor::visit(RootNode* n) 
{
	// for root, just visit all functions
	// we don't have to make a new scope here since symbol table
	// is initialized with an empty scope before the program starts
	for (auto& func : n->funcs)
	{
		func->accept(this);
	}
}

void EvaluateVisitor::visit(BlockNode* n) 
{
	// there is a new scope for a block
	this->symbolTable->PushScope();
	for (auto& stmt : n->stmts)
	{
		stmt->accept(this);
		if (this->needReturn && dynamic_cast<ReturnNode*>(stmt.get()))
		{
			this->hasReturn = true;
		}
	}
	this->symbolTable->PopScope();
}

void EvaluateVisitor::visit(FuncDefnNode* n) 
{
	// Functions have their OWN SYMBOL TABLE! They DO NOT inherit symbols from the calling function!
	this->symbolTable->PushScope();
	this->inFuncDef = true;
	// Let function table know we're going to define a function
	std::string funcname = dynamic_cast<FuncDeclNode*>(n->funcDecl.get())->name;
	n->funcDecl->accept(this);
	this->functionTable->EnterFunctionDefinition(funcname);

	// setup for grabbing the type of the function and check that we're not void , if we are
	// we don't necessarily need a return type
	this->needReturn = false;
	TypeName functype = dynamic_cast<FuncDeclNode*>(n->funcDecl.get())->t;
	if (functype!=TypeName::tVoid)
	{
		this->needReturn = true;
	}
	this->hasReturn = false;

	// evaluate the function body
	n->funcBody->accept(this);

	// if we needed a return statement and we didn't have one, throw an error
	if (this->needReturn && !this->hasReturn)
	{
		std::cout << "Error: Function " << funcname << " is not type void and doesn't have a return statement\n";
		exit(1);
	}

	// The function is now defined
	this->functionTable->DefineFunction(funcname, n->location);
	// make sure that one of the nodes in the body of the function is a return node
	// UNLESS the function is declared void, then we don't need to return
	// FunctionTableEntry* curfunc = this->functionTable->GetFunction(funcname);	

	// tell function table we're done defining a function
	this->inFuncDef = false;
	this->functionTable->ExitFunctionDefinition();
	// tell the symbo table to leave it's current scope
	this->symbolTable->PopScope();
}

void EvaluateVisitor::visit(FuncDeclNode* n) 
{
	// If this is a bare declaration, we don't want these variables to live ina global
	// scope or else something like
	// int foo(int x);
	// int foo2(int x);
	// will cause issues since int x will be declared twice at global scope?
	if (!inFuncDef) 
	{ 
		this->symbolTable->PushScope();
	}
	if (this->functionTable->IsInFunctionDefinition()&&this->functionTable->IsFunctionDefined(n->name))
	{
		// // if we're here, we're a duplicate definition
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Function " << n->name << " already defined.\n";
		FunctionTableEntry* ffunc = this->functionTable->GetFunction(n->name);
		std::cout << "previous definition seen at (" << ffunc->definitionLocation.begin.line << ", " << ffunc->definitionLocation.begin.column << ")\n";
		exit(1);
	}
	// create our parameter types list
	std::vector<TypeName> paramTypes;
	for (auto& param : n->params)
	{
		paramTypes.push_back(param->t);
		param->accept(this);	// visit our declaration to error check and add to symbol table // TODO: Does this work?
								// we can also use these variables in our scope now!
	}
	// TODO: Eventually, this is where we should check that our parameter list matches the function
	// prototypes we've seen already. We don't do that for now.

	// add our function to our function table
	this->functionTable->AddFunction(n->name, n->t, paramTypes);
	if (!inFuncDef)
	{
		this->symbolTable->PopScope();
	}
}

void EvaluateVisitor::visit(FuncCallNode* n) 
{
	// Grab this function from our function table
	FunctionTableEntry* funcResult = this->functionTable->GetFunction(n->name);

	// Make sure we've seen a definition for this function
	if (!funcResult)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Function " << n->name << " not defined\n";
		exit(1);
	}

	// Check we have the same number of arguments in each function
	if (n->funcArgs.size() != funcResult->ParamTypes.size())
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): ";
		std::cout << "Function call argument type mismatch in function " << n->name << "\n";
		std::cout << "Got " << n->funcArgs.size() << " arguments, but expected " << funcResult->ParamTypes.size() << "\n";
		exit(1);
	}

	// Evaluate the type of the function call args
	for (auto& arg : n->funcArgs)
	{
		arg->accept(this);
	}

	// Then, make sure the expected types match
	for (unsigned i = 0; i < n->funcArgs.size(); i++)
	{
		if (n->funcArgs.at(i)->evaluatedType != funcResult->ParamTypes.at(i))
		{
			std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): ";
			std::cout << "Function call argument type mismatch in function " << n->name << "\n";
			std::cout << "Expected type " << TypeNameString(funcResult->ParamTypes.at(i)); 
			std::cout << " but got type " << TypeNameString(n->funcArgs.at(i)->evaluatedType) << "\n";
			exit(1);			
		}
	}

	// Everything looks good.
	// this function call expression will evaluate to the return
	// type of this function
	n->evaluatedType = funcResult->ReturnType;
	n->isConstant = false;
}

void EvaluateVisitor::visit(ConstantIntNode* n) 
{
	// Constant int node is always of type int
	n->evaluatedType = TypeName::tInt;
	n->isConstant = true;
}

void EvaluateVisitor::visit(AssignmentNode* n) 
{
	// do we want to do anything here?
	// make sure lhs and rhs are same type

	n->expr->accept(this);

	// grab this symbol from the symbol table
	SymbolTableEntry* symbolTableEntry;
	symbolTableEntry = this->symbolTable->GetSymbol(n->name);
	// make sure this symbol had been declared already
	if (symbolTableEntry == nullptr)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Undeclared variable: " << n->name <<"\n";
		exit(1);
	}
	// if it is, this expression evaluates to the type of this symbol
	if (symbolTableEntry->Type != n->expr->evaluatedType)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Type mismatch in assignment operation\n";
		std::cout << "Trying to assign type " << TypeNameString(n->expr->evaluatedType);
		std::cout << " to variable of type " << TypeNameString(symbolTableEntry->Type) << "\n";
		exit(1);
	}
}

void EvaluateVisitor::visit(AugmentedAssignmentNode* n) 
{
	// do we need to do anything here?

	n->expr->accept(this);

	// grab this symbol from the symbol table
	SymbolTableEntry* symbolTableEntry;
	symbolTableEntry = this->symbolTable->GetSymbol(n->name);
	// make sure this symbol had been declared already
	if (symbolTableEntry == nullptr)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Undeclared variable: " << n->name <<"\n";
		exit(1);
	}
	// if it is, this expression evaluates to the type of this symbol
	if (symbolTableEntry->Type != n->expr->evaluatedType)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Type mismatch in augmented assignment operation\n";
		std::cout << "Trying to assign type " << TypeNameString(n->expr->evaluatedType);
		std::cout << " to variable of type " << TypeNameString(symbolTableEntry->Type) << "\n";
		exit(1);
	}
}

void EvaluateVisitor::visit(BoolNode* n) 
{
	// Constant bool node is always of type bool
	n->evaluatedType = TypeName::tBool;
	n->isConstant = true;
}

void EvaluateVisitor::visit(ReturnNode* n) 
{
	// Get our current function context
	FunctionTableEntry* curfunc = this->functionTable->GetCurrentFunction();
	// If we don't have a current function, we can't return outside a function. This is actually a syntax error but
	// I guess we'll double check here in case something tricky has happened?
	if (!curfunc)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Return outside of function definition\n";
		exit(1);
	}
	// If our return function has an expression, figure out it's type
	if (n->expr)
	{
		n->expr->accept(this);
	}
	// If we have an expression, grab our return type from that, otherwise our return type is void
	TypeName rtype = n->expr?n->expr->evaluatedType:TypeName::tVoid;
	// Check for mismatch between return expression and expected type
	if (rtype != curfunc->ReturnType)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Return type mistmatch in function " << curfunc->Name <<"\n";
		std::cout << "Expected return type " << TypeNameString(curfunc->ReturnType) << " but got " << TypeNameString(rtype) << "\n";
		exit(1);
	}
}

void EvaluateVisitor::visit(ConstantFloatNode* n) 
{
	// constant float node is always of type float
	n->evaluatedType = TypeName::tFloat;
	n->isConstant = true;
}

void EvaluateVisitor::visit(IfNode* n) 
{
	// Evaluate if condition to make sure it evaluates to bool
	n->ifExpr->accept(this);
	if (n->ifExpr->evaluatedType != TypeName::tBool)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Condition of if statement must be a boolean\n";
		std::cout << "but it evaluated to " << TypeNameString(n->ifExpr->evaluatedType) << "\n";
		exit(1);
	}
	// Evaluate the body of the if loop, this will create a new scope as well
	n->ifBody->accept(this);
	// Exit scope once we're done evaluating the function body
}

void EvaluateVisitor::visit(ForNode* n) 
{
	// What else do we need to consider in this for loop?
	// we start a new scope here so we can't redefine loop vars we use
	this->symbolTable->PushScope();
	// If we have an init statement, check it ou
	if (n->initStmt)
	{
		n->initStmt->accept(this);
	}
	// If we have a predicate, evaluate predicate to make sure it evaluates to bool
	if (n->loopCondExpr)
	{
		n->loopCondExpr->accept(this);
		if (n->loopCondExpr->evaluatedType != TypeName::tBool)
		{
			std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Condition of for statement must be a boolean\n";
			std::cout << "but it evaluated to " << TypeNameString(n->loopCondExpr->evaluatedType) << "\n";
			exit(1);
		}
	}
	// If we have a conditional, check it out
	if (n->updateStmt)
	{
		n->updateStmt->accept(this);
	}
	// check rest of for loop
	n->loopBody->accept(this);
	this->symbolTable->PopScope();
}

void EvaluateVisitor::visit(WhileNode* n) 
{
	// What else do we need to handle in this while block

	// Evaluate predicate of while loop to make sure it evaluates to bool
	n->whileExpr->accept(this);
	if (n->whileExpr->evaluatedType != TypeName::tBool)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Condition of while statement must be a boolean\n";
		std::cout << "but it evaluated to " << TypeNameString(n->whileExpr->evaluatedType) << "\n";
		exit(1);
	}
	// Evaluate body of while loop
	n->loopBody->accept(this);
}

void EvaluateVisitor::visit(UnaryNode* n) 
{
	// Type of a Unary node is determined by the type of its
	// subexpression, ie -int vs. -float
	n->expr->accept(this);
	n->evaluatedType = n->expr->evaluatedType;
	// A unary node is constant if it's expression is constant
	n->isConstant = n->expr->isConstant;
}

void EvaluateVisitor::visit(TernaryNode* n) 
{
	// Make sure predicate of ternary expression evaluates to bool
	n->condExpr->accept(this);
	if (n->condExpr->evaluatedType != TypeName::tBool)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Condition of ternary expression must be boolean.\n";
		std::cout << "but it evaluated to " << TypeNameString(n->condExpr->evaluatedType) << "\n";
		exit(1);
	}
	// evaluate branches of a ternary expression
	n->trueExpr->accept(this);
	n->falseExpr->accept(this);
	// check that they are the same type
	if (n->trueExpr->evaluatedType != n->falseExpr->evaluatedType)
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Type mismatch in ternary operands\n";
		std::cout << "Got types " << TypeNameString(n->trueExpr->evaluatedType);
		std::cout << " and " << TypeNameString(n->falseExpr->evaluatedType) << "\n";
		exit(1);
	}
	// If we're here, our type matches our children
	n->evaluatedType = n->trueExpr->evaluatedType;
	// if both operands are constant, it's good
	n->isConstant = (n->trueExpr->isConstant && n->falseExpr->isConstant);

}

void EvaluateVisitor::visit(CastExpressionNode* n) 
{
	if (!(n->t == TypeName::tInt || n->t == TypeName::tFloat))
	{
		std::cout << "Error (" << n->location.begin.line << ", " << n->location.begin.column << "): Can only cast between float and integer\n";
		std::cout << "But tried to cast to type " << TypeNameString(n->t) << "\n";
		exit(1);
	}
	// a cast expression evaluate to the type we are casting to
	n->evaluatedType = n->t;
	// we're only constant if the expression we are casting is constant
	n->expr->accept(this);
	n->isConstant = n->expr->isConstant;
}

void EvaluateVisitor::visit(ExpressionStatementNode* n)
{
	// This is an expression that is wrapped in a statement
	// we evaluate the expression in case there are any side
	// effects, but this node won't evaluate to anything
	n->expr->accept(this);
}

void EvaluateVisitor::visit(BreakNode* n) 
{
	// hmmm? Here we would probably have a link to whatever
	// statement we are in. 
}

void EvaluateVisitor::visit(ContinueNode* n) 
{
	// hmmm? Here we would probably link to whatever statement
	// we are in.
}	

#include "vcodegen.hpp"
#include "nodes.hpp"
#include "common.hpp"
#include <memory>
#include <iostream>
#include <string>

// TODO: Figure out what we need/don't here
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/ValueHandle.h"

// Error generating
#include "llvm/Support/Error.h"

/*
Basic block notes:
- EVERY basic block MUST end with a terminator instruction:
	- ret - return control flow to calling function
		- two forms of return statement: ret <type> <value> or ret void
	- br, switch, indirect br - transfer control to another BB in the same function
	- invoke - transfer control to another function

*/

CodegenVisitor::CodegenVisitor()
{
	// pass
}

llvm::Value* CodegenVisitor::consumeRetValue()
{
	llvm::Value* retval = this->retValue;
	this->retValue = nullptr;
	return retval;
}

void CodegenVisitor::setRetValue(llvm::Value* v)
{
	this->retValue = v;
}

void CodegenVisitor::visit(VariableNode* n) 
{
	// Value *VariableExprAST::codegen() {
	// // Look this variable up in the function.
	// Value *V = NamedValues[Name];
	// if (!V)
	// 	LogErrorV("Unknown variable name");
	// return V;
	// }

	llvm::Value* val = this->symbolTable[n->name];
	if (!val)
	{
		// TODO: error checking unknown variable name
	}
	this->setRetValue(val);
}

void CodegenVisitor::visit(DeclarationNode* n) 
{

	// std::string name;
	// TypeName t;

	// install a symbol
	// llvm::Type::getIntNTy(context, num_bits)	// Create integer type
}

void CodegenVisitor::visit(DeclAndAssignNode* n) 
{
	// std::unique_ptr<DeclarationNode> decl;	// declaration
	// std::unique_ptr<ExpressionNode> expr;	// expression to be assigned to the variable

	// install a symbol and set it's value
	// we should just use the mechanisms implemented in declaration and assign here
}

void CodegenVisitor::visit(BinaryOpNode* n) 
{
	// binary, logical, and relational op are all basically the same thing

	// BinaryOps op;
	// std::unique_ptr<ExpressionNode> left;
	// std::unique_ptr<ExpressionNode> right;
	// llvm::Value* operator = GetLLVMBinaryOp(n->op, &(this->compilationUnit->builder));
	n->left->accept(this);
	llvm::Value* lval = this->consumeRetValue();
	n->right->accept(this);
	llvm::Value* rval = this->consumeRetValue();
	// this->retValue = GetLLVMBinaryOp(n->op, lval, rval);
	// this->setRetValue(GetLLVMBinaryOp(n->op, lval, rval));
	if (n->evaluatedType == TypeName::tFloat)
	{
		this->setRetValue(GetLLVMBinaryOpFP(n->op, lval, rval));
	} 
	else
	{
		this->setRetValue(GetLLVMBinaryOpInt(n->op, lval, rval));
	}
}

void CodegenVisitor::visit(LogicalOpNode* n) 
{
	// binary, logical, and relational op are all basically the same thing
	// BinaryOps op;
	// std::unique_ptr<ExpressionNode> left;
	// std::unique_ptr<ExpressionNode> right;
	n->left->accept(this);
	llvm::Value* lval = this->consumeRetValue();
	n->right->accept(this);
	llvm::Value* rval = this->consumeRetValue();
	// this->retValue = GetLLVMBinaryOp(n->op, lval, rval);
	this->setRetValue(GetLLVMBinaryOpInt(n->op, lval, rval));
	// 	LogAnd,
	// 	LogOr
}

void CodegenVisitor::visit(RelationalOpNode* n) 
{	
	// binary, logical, and relational op are all basically the same thing
	// RelationalOps op;
	// std::unique_ptr<ExpressionNode> left;
	// std::unique_ptr<ExpressionNode> right;
	n->left->accept(this);
	llvm::Value* lval = this->consumeRetValue();
	n->right->accept(this);
	llvm::Value* rval = this->consumeRetValue();
	// We only have to check the left nodes evaluated type since, due to semantic checking, left and right
	// are guaranteed to have matching types by the time we get here
	if (n->left->evaluatedType == TypeName::tFloat)
	{
		// Floating point comparison
		this->setRetValue(GetLLVMRelationalOpFP(n->op, lval, rval));
	}
	else
	{
		// Integer comparison
		this->setRetValue(GetLLVMRelationalOpInt(n->op, lval, rval));
	}
}

void CodegenVisitor::visit(RootNode* n) 
{
	// For a root, we just visit every function in our function list
	for (auto& func : n->funcs)
	{
		func->accept(this);
	}
}

void CodegenVisitor::visit(BlockNode* n) 
{
	// std::vector<std::unique_ptr<Node>> stmts;
	for (auto& stmt : n->stmts)
	{
		stmt->accept(this);
	}
	// A block means we have to generate a new scope!
}

void CodegenVisitor::visit(FuncDefnNode* n) 
{
	// What should we do if we have an external function foo and a defined function foo?
	// We need to ensure that a function is empty before we starting filling out its body.
	
	// For now we'll assume the function has not been externally defined before reaching this
	// definition

	// Do codegen on the function declaration, then get the function object from the module
	n->funcDecl->accept(this);
	llvm::Function* f = this->compilationUnit->module->getFunction(n->funcDecl->name);

	// // Create a new basic block to start insertion into.
	llvm::BasicBlock *BB = llvm::BasicBlock::Create(*(this->compilationUnit->context.get()), "entry", f);
	this->compilationUnit->builder.SetInsertPoint(BB);

	// Add parameters to our symbol table
	this->symbolTable.clear();
	for (auto &Arg : f->args())
	{
		this->symbolTable[Arg.getName()] = &Arg;
	}

	n->funcBody->accept(this);
	if (n->funcDecl->t == TypeName::tVoid)
	{
		// If the type of this function is void, don't return anything
		this->compilationUnit->builder.CreateRet(nullptr);
	}
	else
	{
		// otherwise, whatever we return is based whatever evaluating the function
		// body spat out
		this->compilationUnit->builder.CreateRet(this->consumeRetValue());
	}

	// Here, we start creating actual blocks and inserting them into the module via the builder (?!)
}

void CodegenVisitor::visit(FuncDeclNode* n) 
{
	// Generate our parameter LLVM types from our AST parameter types
	std::vector<llvm::Type*> parameters;
	for (int i = 0; i < (int) n->params.size(); i++) 
	{
		parameters.push_back(GetLLVMType(
			n->params[i]->t
		));
	}

	// Create our LLVM function signature
	llvm::FunctionType* signature = llvm::FunctionType::get(
		GetLLVMType(n->t) /*return type*/,
		parameters /*std::vector<Type*> paramTypes*/,
		false
	);

	// Now, create the actual entry in the function table
	llvm::Function* f = llvm::Function::Create(
		signature,
		llvm::Function::ExternalLinkage,
		n->name,
		this->compilationUnit->module.get()
	);

	// Name our function parameters
	unsigned int i = 0;
	for (llvm::Argument& a : f->args()) {
		a.setName(n->params[i++]->name);
	}
}

void CodegenVisitor::visit(FuncCallNode* n) 
{
	llvm::Function* CalleeF = this->compilationUnit->module->getFunction(n->name);
	if (!CalleeF)
	{
		// TODO: This is an error! We can't find the function we've referenced
	}

	std::vector<llvm::Value *> ArgsV;
	for (unsigned i = 0, e = n->funcArgs.size(); i != e; ++i)
	{
		n->funcArgs[i]->accept(this);
		ArgsV.push_back(this->consumeRetValue());
		if (!ArgsV.back())
		{
			// TODO: This is an error, problem with evaluating expression
		}
	}

	this->setRetValue(this->compilationUnit->builder.CreateCall(CalleeF, ArgsV, "call_" + n->name));
}

void CodegenVisitor::visit(ConstantIntNode* n) 
{
	// Set return value to be a constant integer node
	this->setRetValue(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*(this->compilationUnit->context.get())), n->intValue, true));
}

void CodegenVisitor::visit(AssignmentNode* n) 
{

	// std::string name;						// can be name or declaration
	// std::unique_ptr<ExpressionNode> expr;	// the expression we are going to assign to name

	// get a pointer to a symbol from the namedvalues and assign a value to it
}

void CodegenVisitor::visit(AugmentedAssignmentNode* n) 
{
	// AugmentedAssignOps op;
	// std::string name;
	// std::unique_ptr<ExpressionNode> expr;	// the expression we are going to assign to name
	// {
	// 	case AugmentedAssignOps::PlusEq:
	// 		return "+=";
	// 	case AugmentedAssignOps::MinusEq:
	// 		return "-=";
	// 	case AugmentedAssignOps::StarEq:
	// 		return "*=";
	// 	case AugmentedAssignOps::SlashEq:
	// 		return "/=";
	// }

	// can use the same mechanism as Assignment, since x += 1 ie is just syntactic sugar for x = x + 1
}

void CodegenVisitor::visit(BoolNode* n) 
{
	// llvm::ConstantFP::get(llvm::Type::getInt1Ty(context), value);
	// 
	const unsigned int v = n->boolValue ? 1 : 0;
	this->setRetValue(llvm::ConstantInt::get(llvm::Type::getInt1Ty(*(this->compilationUnit->context.get())), v));	// do we need true/false here?
}

void CodegenVisitor::visit(ReturnNode* n) 
{
	// std::unique_ptr<ExpressionNode> expr;
	// insert ret into our basic block here but first we need to evaluate the expression to 
	// figure out what it's value and type are.
	if (n->expr)
		n->expr->accept(this);
	else
		this->setRetValue(nullptr);
}

void CodegenVisitor::visit(ConstantFloatNode* n) 
{
	// Set retValue to be a constant floating point number
	this->setRetValue(llvm::ConstantFP::get(llvm::Type::getFloatTy(*(this->compilationUnit->context.get())), n->floatValue));// do we need true/false here?
}

void CodegenVisitor::visit(IfNode* n) 
{
	// this->ifExpr = std::move(ifExpr);
	// this->ifBody = std::move(ifBody);

	// basic block, compare and jump on comparison success or fail?

	// condition:
	// evaluate if condition
	//	- builder.CreateCondBr(val, TrueBB, FalseBB)
	// branch if false

	// if code goes here...

	// false branch jumps here...

}

void CodegenVisitor::visit(ForNode* n) 
{

	// this->initStmt = std::move(initStmt);
	// this->midExpr = std::move(midExpr);
	// this->loopCondStmt = std::move(loopCondStmt);
	// this->loopBody = std::move(loopBody);

	// this is described in the lab handout
	// 	before:
	// 	// whatever happens before the loop
	// 	goto loop_condition

	// 	header:
	// 	// loop induction statement
	// 	goto loop_condition

	// 	loop_condition:
	// 	// evaluate loop condition
	// 	branch body after

	// 	body:
	// 	// loop body
	// 	goto header
	//		- builder.CreateBR(label);	// unconditional branch

	// 	after:
	// // continue generating code
}

void CodegenVisitor::visit(WhileNode* n) 
{

	// this->whileExpr = std::move(whileExpr);
	// this->loopBody = std::move(loopBody);

	// while loop head:
	// evaluate loop condition:
	// if false jump to end while loop
	// while code...
	// jump to while loop head:
	// end while loop:
	// ...
}

void CodegenVisitor::visit(UnaryNode* n) 
{
	// evaluate the sub expression
	// create multiplication -1 and this->retValue
}

void CodegenVisitor::visit(TernaryNode* n) 
{

	// this->condExpr = std::move(condExpr);
	// this->trueExpr = std::move(trueExpr);
	// this->falseExpr = std::move(falseExpr);

	// this is the same setup as an if statement but then we make sure the expression
	// evaluates to whichever branch tests true.
}

void CodegenVisitor::visit(CastExpressionNode* n) 
{
	// this->t = t;
	// this->expr = std::move(expr);

	// hmmm. how is casting supposed to work?
	// in this language we ONLY cast integer <-> float

	// by the time we get here we should KNOW that this cast is between
	// a float and an int
	if (n->t == TypeName::tInt)
	{
		// we're casting from a float to an integer
		n->expr->accept(this);
		this->setRetValue(this->compilationUnit->builder.CreateFPToSI(this->consumeRetValue(), this->compilationUnit->builder.getInt32Ty()));
	}
	else
	{
		// we'r casting from an integer to a float
		n->expr->accept(this);
		this->setRetValue(this->compilationUnit->builder.CreateSIToFP(this->consumeRetValue(), this->compilationUnit->builder.getFloatTy()));
	}
}

void CodegenVisitor::visit(ExpressionStatementNode* n)
{
	// evaluate the subexpression (in case of side effects)
	// but then we can just toss the results!	
	n->expr->accept(this);
	this->setRetValue(nullptr);
}

void CodegenVisitor::visit(BreakNode* n) 
{
	// this node does not hold any information right now, does it have to? Or is this something we can 
	// grab through llvm somehow?

	// break out of top level loop expression
	// this seems a little wonky, how do we manage this one? jump back to the closest
	// end: statement in the same scope?
}

void CodegenVisitor::visit(ContinueNode* n) 
{
	// this node currently does not hold any info, is this something we can grab through llvm or should
	// we be storing it in the AST somehow?!

	// jump straight back to the closest above us start: statement in the same scope
}	

llvm::Value* CodegenVisitor::GetLLVMRelationalOpInt(RelationalOps r, llvm::Value* lhs, llvm::Value* rhs)
{
	// translates from relational ops used in AST gen/semantic analysis into llvm native funcs
	switch(r) 
	{
		case RelationalOps::Eq:
			return this->compilationUnit->builder.CreateICmpEQ(lhs, rhs);
		case RelationalOps::Ne:
			return this->compilationUnit->builder.CreateICmpNE(lhs, rhs);
		case RelationalOps::Lt:
			return this->compilationUnit->builder.CreateICmpSLT(lhs, rhs);
		case RelationalOps::Gt:
			return this->compilationUnit->builder.CreateICmpSGT(lhs, rhs);
		case RelationalOps::Le:
			return this->compilationUnit->builder.CreateICmpSLE(lhs, rhs);
		case RelationalOps::Ge:
			return this->compilationUnit->builder.CreateICmpSGE(lhs, rhs);
		default:
			llvm_unreachable("Invalid relational operator");
			return nullptr;
	}
}

llvm::Value* CodegenVisitor::GetLLVMRelationalOpFP(RelationalOps r, llvm::Value* lhs, llvm::Value* rhs)
{
	// translates from relational ops used in AST gen/semantic analysis into llvm native funcs
	switch(r) 
	{
		case RelationalOps::Eq:
			return this->compilationUnit->builder.CreateFCmpOEQ(lhs, rhs);
		case RelationalOps::Ne:
			return this->compilationUnit->builder.CreateFCmpONE(lhs, rhs);
		case RelationalOps::Lt:
			return this->compilationUnit->builder.CreateFCmpOLT(lhs, rhs);
		case RelationalOps::Gt:
			return this->compilationUnit->builder.CreateFCmpOGT(lhs, rhs);
		case RelationalOps::Le:
			return this->compilationUnit->builder.CreateFCmpOLE(lhs, rhs);
		case RelationalOps::Ge:
			return this->compilationUnit->builder.CreateFCmpOGE(lhs, rhs);
		default:
			llvm_unreachable("Invalid relational operator");
			return nullptr;
	}
}

llvm::Value* CodegenVisitor::GetLLVMBinaryOpInt(BinaryOps b, llvm::Value* lhs, llvm::Value* rhs)
{
	// translate from BinaryOp enums used in AST/semantic analysis into llvms native functions
	switch(b) 
	{
		case BinaryOps::Plus:
			return this->compilationUnit->builder.CreateAdd(lhs, rhs);
		case BinaryOps::Minus:
			return this->compilationUnit->builder.CreateSub(lhs, rhs);
		case BinaryOps::Star:
			// return this->compilationUnit->builder.CreateFMul(lhs, rhs);
			return this->compilationUnit->builder.CreateMul(lhs, rhs);
		case BinaryOps::Slash:
			return this->compilationUnit->builder.CreateSDiv(lhs, rhs);
		case BinaryOps::LogAnd:
			return this->compilationUnit->builder.CreateAnd(lhs, rhs);
		case BinaryOps::LogOr:
			return this->compilationUnit->builder.CreateOr(lhs, rhs);
		default:
			llvm_unreachable("Invalid binary operator");
			return nullptr;
	}
}

llvm::Value* CodegenVisitor::GetLLVMBinaryOpFP(BinaryOps b, llvm::Value* lhs, llvm::Value* rhs)
{
	// translate from BinaryOp enums used in AST/semantic analysis into llvms native functions
	switch(b) 
	{
		case BinaryOps::Plus:
			return this->compilationUnit->builder.CreateFAdd(lhs, rhs);
		case BinaryOps::Minus:
			return this->compilationUnit->builder.CreateFSub(lhs, rhs);
		case BinaryOps::Star:
			return this->compilationUnit->builder.CreateFMul(lhs, rhs);
			// return this->compilationUnit->builder.CreateMul(lhs, rhs);
		case BinaryOps::Slash:
			return this->compilationUnit->builder.CreateFDiv(lhs, rhs);
		default:
			llvm_unreachable("Invalid binary operator");
			return nullptr;
	}
}


llvm::Type* CodegenVisitor::GetLLVMType(TypeName t)
{
	// Translate from the TypeName enums used by the AST and semantic analysis into
	// llvm's native types
	switch(t) 
	{
		case TypeName::tVoid:
			return this->compilationUnit->builder.getVoidTy();
		case TypeName::tInt:
			return this->compilationUnit->builder.getInt32Ty();
		case TypeName::tFloat:
			return this->compilationUnit->builder.getFloatTy();
		case TypeName::tBool:
			return this->compilationUnit->builder.getInt1Ty();
		default:
			llvm_unreachable("Invalid type");
			return nullptr;
	}
}
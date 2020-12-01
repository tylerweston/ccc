#include "vcodegen.hpp"
#include "nodes.hpp"
#include "common.hpp"
#include "symtable.hpp"
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
	symTable = new SymbolTable();
}

CodegenVisitor::~CodegenVisitor()
{
	symTable->CleanUpSymbolTable();
	delete(symTable);
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

	llvm::AllocaInst* val = this->symTable->GetLLVMValue(n->name); //this->symbolTable[n->name];
	if (!val)
	{
		// TODO: error checking unknown variable name
	}
	llvm::Value* r = this->compilationUnit->builder.CreateLoad(val, n->name);
	this->setRetValue(r);
}

void CodegenVisitor::visit(DeclarationNode* n) 
{

	// std::string name;
	// TypeName t;

	// install a symbol
	// llvm::Type::getIntNTy(context, num_bits)	// Create integer type
	llvm::AllocaInst* Alloca = this->compilationUnit->builder.CreateAlloca(
			this->GetLLVMType(n->t), 0, n->name
		);
	this->symTable->AddLLVMSymbol(n->name, Alloca);
	// this->retValue = Alloca;
}

void CodegenVisitor::visit(DeclAndAssignNode* n) 
{
	// install a symbol and set it's value
	llvm::AllocaInst* Alloca = this->compilationUnit->builder.CreateAlloca(
			this->GetLLVMType(n->decl->t), 0, n->decl->name
		);
	this->symTable->AddLLVMSymbol(n->decl->name, Alloca);
	n->expr->accept(this);
	this->compilationUnit->builder.CreateStore(this->consumeRetValue(), Alloca);
}

void CodegenVisitor::visit(BinaryOpNode* n) 
{
	// Generate code to evaluate binary operations

	// Evaluate our two operands
	n->left->accept(this);
	llvm::Value* lval = this->consumeRetValue();
	n->right->accept(this);
	llvm::Value* rval = this->consumeRetValue();

	// Either perform floating point or int math depending on the type of this node
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
	// Generate code to evaluate logical operations

	// Evaluate our two operands
	n->left->accept(this);
	llvm::Value* lval = this->consumeRetValue();
	n->right->accept(this);
	llvm::Value* rval = this->consumeRetValue();
	// Perform our logical operation
	this->setRetValue(GetLLVMBinaryOpInt(n->op, lval, rval));
}

void CodegenVisitor::visit(RelationalOpNode* n) 
{	
	// Generate code to handle relational operations

	// Evaluate our two operands
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
	// A block means we have to generate a new scope!
	this->symTable->PushScope();
	for (auto& stmt : n->stmts)
	{
		stmt->accept(this);
		// TODO: IF we get a return at the top level before the function is finished, we
		// need to not generate the rest of the function!
	}
	// when we leave a block, we need a return function
	// at a higher level
	this->symTable->PopScope();
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

	// Create a new basic block to start insertion into.
	llvm::BasicBlock *BB = llvm::BasicBlock::Create(*(this->compilationUnit->context.get()), "entry", f);
	this->compilationUnit->builder.SetInsertPoint(BB);

	// Add parameters to our symbol table and reserve room on our stack for them. Note that each function
	// is also it's own scope, so we push scope here and pop scope when this function is finished.
	this->symTable->PushScope();
	for (auto &Arg : f->args())
	{
		llvm::AllocaInst *Alloca = this->CreateEntryBlockAlloca(f, Arg.getName(), Arg.getType());
		this->compilationUnit->builder.CreateStore(&Arg, Alloca);
		this->symTable->AddLLVMSymbol(Arg.getName(), Alloca);
	}

	// Evaluate the body of this function
	n->funcBody->accept(this);

	// Now, this might not always be correct? 
	//  - What about returns in the middle of a function?
	//  - What about returns in the middle of control blocks?
	//  - We should probably be actually inserting the ret operation when
	//    we encounter a return node. If we just insert it, it should(?)
	//    be in the correct place in the correct basic block?
	// if (n->funcDecl->t == TypeName::tVoid)
	// {
	// 	// If the type of this function is void, don't return anything
	// 	this->compilationUnit->builder.CreateRet(nullptr);
	// }
	// else
	// {
	// 	// otherwise, whatever we return is based whatever evaluating the function
	// 	// body spat out
	// 	this->compilationUnit->builder.CreateRet(this->consumeRetValue());
	// }
	if (this->compilationUnit->builder.GetInsertBlock()->getTerminator() == nullptr)
	{
		// If we don't have a return at the end of a block, it is is because we are in a void
		// function and it doesn't have a final return value
		this->compilationUnit->builder.CreateRet(nullptr);
	}
	// Pop the scope for this function, discarding the values
	this->symTable->PopScope();	// TODO: Do we need to clean up a scope before we pop it?
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
	// TODO: Maybe custom names for non-void return functions
	this->setRetValue(this->compilationUnit->builder.CreateCall(CalleeF, ArgsV));
}

void CodegenVisitor::visit(ConstantIntNode* n) 
{
	// Set return value to be a constant integer node
	this->setRetValue(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*(this->compilationUnit->context.get())), n->intValue, true));
}

void CodegenVisitor::visit(AssignmentNode* n) 
{
	// get a pointer to a symbol from the namedvalues and assign a value to it
	llvm::AllocaInst* lloc = this->symTable->GetLLVMValue(n->name);
	if (!lloc)
	{
		// TODO: error checking here
	}
	n->expr->accept(this);
	this->compilationUnit->builder.CreateStore(this->consumeRetValue(), lloc);
}

void CodegenVisitor::visit(AugmentedAssignmentNode* n) 
{

	// First, we'll grab the variable
	// figure out the operation associated with the AugmentedAssignOps
	// apply this operation to the variable
	// then store it back where it used to be

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
	// Generate a constant boolean value, represented as a 1 bit integer in llvm
	const unsigned int v = n->boolValue ? 1 : 0;
	this->setRetValue(llvm::ConstantInt::get(llvm::Type::getInt1Ty(*(this->compilationUnit->context.get())), v));
}

void CodegenVisitor::visit(ReturnNode* n) 
{
	// insert ret into our basic block here but first we need to evaluate the expression to 
	// figure out what it's value and type are. If there is no expression, it is a void return
	// so we just return a nullptr
	if (n->expr)
	{
		n->expr->accept(this);
		this->compilationUnit->builder.CreateRet(this->consumeRetValue());
	}
	else
	{
		this->compilationUnit->builder.CreateRet(nullptr);
	}
		//this->setRetValue(nullptr);
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

	// OK first we evaluate the if condition
	n->ifExpr->accept(this);
	llvm::Value* condV = this->consumeRetValue();
	if (!condV)
	{
		// TODO: Error out here!
	}
	// now, we turn this condition into an bool (int1) by neq'ing it with 0
	condV = this->compilationUnit->builder.CreateICmpNE(
		condV, 
		llvm::ConstantInt::get(llvm::Type::getInt1Ty(*(this->compilationUnit->context.get())), 0), 
		"ifcond"
	);

	// Figure out which block we're currently in and where to insert the if statements
	llvm::Function *theFunction = this->compilationUnit->builder.GetInsertBlock()->getParent();

	// Since we don't support else, we only have two blocks, IF TRUE and IF CONTINUTE (ie, false)
	llvm::BasicBlock *iftrueBB = llvm::BasicBlock::Create(*(this->compilationUnit->context.get()), "iftrue", theFunction);
	llvm::BasicBlock *ifcontBB = llvm::BasicBlock::Create(*(this->compilationUnit->context.get()), "ifcont");

	// conditionally jump to iftrue or ifcont 
	this->compilationUnit->builder.CreateCondBr(condV, iftrueBB, ifcontBB);

	// generate code for if body
	this->compilationUnit->builder.SetInsertPoint(iftrueBB);
	n->ifBody->accept(this);
	if (this->compilationUnit->builder.GetInsertBlock()->getTerminator() == nullptr)
	{
		// If we returned in this branch, we don't want to BR out of it
		this->compilationUnit->builder.CreateBr(ifcontBB);
	}

	// push if continue block and make that our new insert point to continue code generation
	theFunction->getBasicBlockList().push_back(ifcontBB);
	this->compilationUnit->builder.SetInsertPoint(ifcontBB);
	// TODO: Do we have to deal with any Phi stuff here? Or is that all sorted
	// out by the llvm? Seems to be working fine?
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
	// assume the type of the child is float or int since -bool or -void doesn't really make
	// any sense!
	// create multiplication -1 and this->retValue
}

void CodegenVisitor::visit(TernaryNode* n) 
{
	// this->condExpr = std::move(condExpr);
	// this->trueExpr = std::move(trueExpr);
	// this->falseExpr = std::move(falseExpr);

	// this is the same setup as an if statement but then we make sure the expression
	// evaluates to whichever branch tests true.

	// we evaluate our condExpr, then based on retvalue, 
	// either set to be the true or false branch
}

void CodegenVisitor::visit(CastExpressionNode* n) 
{
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

	// break out a loop by jumping to the after label
	// To handle break and continue, you must keep track of the "header" and "after" block for each loop.
	// so in codegen we have a "header" and "after" labels that we update appropriately whenever we create
	// a new loop construct?
}

void CodegenVisitor::visit(ContinueNode* n) 
{
	// this node currently does not hold any info, is this something we can grab through llvm or should
	// we be storing it in the AST somehow?!

	// jump straight back to the current header label
	// To handle break and continue, you must keep track of the "header" and "after" block for each loop.
	// so in codegen we have a "header" and "after" labels that we update appropriately whenever we create
	// a new loop construct?
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

llvm::AllocaInst* CodegenVisitor::CreateEntryBlockAlloca(llvm::Function* TheFunction, std::string VarName, llvm::Type* t)
{
	llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
		TheFunction->getEntryBlock().begin());
	return TmpB.CreateAlloca(t, 0, VarName);
}
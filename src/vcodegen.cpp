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

CodegenVisitor::CodegenVisitor()
{
	// Create a new symbol table
	symTable = new SymbolTable();
}

CodegenVisitor::~CodegenVisitor()
{
	// Clean up symbol table
	symTable->CleanUpSymbolTable();
	delete(symTable);
}

llvm::Value* CodegenVisitor::consumeRetValue()
{
	// This is the equivalent of returning a value from a function
	// consuming it is to use that return value
	llvm::Value* retval = this->retValue;
	this->retValue = nullptr;
	return retval;
}

void CodegenVisitor::setRetValue(llvm::Value* v)
{
	// set the return value to be consumed later
	this->retValue = v;
}

void CodegenVisitor::visit(VariableNode* n) 
{
	// Lookup a variable in the symbol table and grab it's location
	llvm::AllocaInst* val = this->symTable->GetLLVMValue(n->name);
	if (!val)
	{
		std::cout << "Error: Variable " << n->name << " not found.\n";
		exit(1);
	}
	// Load the value expected from that location and return int
	llvm::Value* r = this->compilationUnit->builder.CreateLoad(val, n->name);
	this->setRetValue(r);
}

void CodegenVisitor::visit(DeclarationNode* n) 
{

	// Allocate space for a variable of the specified name and type
	// and add it to the symbol table
	llvm::AllocaInst* Alloca = this->compilationUnit->builder.CreateAlloca(
			this->GetLLVMType(n->t), 0, n->name
		);
	this->symTable->AddLLVMSymbol(n->name, Alloca);
}

void CodegenVisitor::visit(DeclAndAssignNode* n) 
{
	// Allocate space for a variable and set it's value
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
		// need to not generate the rest of the function!?
	}
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

	// make sure we have our returns setup properly
	if (this->compilationUnit->builder.GetInsertBlock()->getTerminator() == nullptr)
	{
		// If we don't have a return at the end of a block, it is is because we are in a void
		// function and it doesn't have a final return value
		this->compilationUnit->builder.CreateRet(nullptr);
	}
	// Pop the scope for this function, discarding the values
	this->symTable->PopScope();
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
	// Call a function
	llvm::Function* CalleeF = this->compilationUnit->module->getFunction(n->name);
	if (!CalleeF)
	{
		std::cout << "Error: Can't find function named " << n->name<< "\n";
		exit(1);
	}

	std::vector<llvm::Value *> ArgsV;
	for (unsigned i = 0, e = n->funcArgs.size(); i != e; ++i)
	{
		n->funcArgs[i]->accept(this);
		ArgsV.push_back(this->consumeRetValue());
		if (!ArgsV.back())
		{
			// TODO: This is an error, problem with evaluating expression
			std::cout << "Error: Problem with parameter number " << i << " in function " << n->name << "\n";
			exit(1);
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
		std::cout << "Error: Can't find variable named " << n->name << "\n";
		exit(1);
	}
	n->expr->accept(this);
	this->compilationUnit->builder.CreateStore(this->consumeRetValue(), lloc);
}

void CodegenVisitor::visit(AugmentedAssignmentNode* n) 
{

	// First, we'll grab the variable
	llvm::AllocaInst* lloc = this->symTable->GetLLVMValue(n->name);
	if (!lloc)
	{
		std::cout << "Error: Can't find variable named " << n->name << "\n";
		exit(1);
	}
	llvm::Value* lval = this->compilationUnit->builder.CreateLoad(lloc, n->name);


	// then evalute the rhs
	n->expr->accept(this);
	llvm::Value* rval = this->consumeRetValue();
	// apply this operation to the variable
	// Either perform floating point or int math depending on the type of this node
	if (n->expr->evaluatedType == TypeName::tInt)
	{
		this->setRetValue(GetLLVMAugmentedAssignOpsInt(n->op, lval, rval));
	} 
	else
	{
		this->setRetValue(GetLLVMAugmentedAssignOpsFP(n->op, lval, rval));
	}

	// then store it back where it used to be
	this->compilationUnit->builder.CreateStore(this->consumeRetValue(), lloc);
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
}

void CodegenVisitor::visit(ConstantFloatNode* n) 
{
	// Set retValue to be a constant floating point number
	this->setRetValue(llvm::ConstantFP::get(llvm::Type::getFloatTy(*(this->compilationUnit->context.get())), n->floatValue));// do we need true/false here?
}

void CodegenVisitor::visit(IfNode* n) 
{
	// first we evaluate the if condition
	n->ifExpr->accept(this);
	llvm::Value* condV = this->consumeRetValue();
	if (!condV)
	{
		std::cout << "Error: Can't evaluate condition of if statement\n";
		exit(1);
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
}

void CodegenVisitor::visit(ForNode* n) 
{
	// TODO: Should we PushScope() here and PopScope() at the end?
	// I don't think so since we create a new scope in the body of the for loop anyways
	// Yes! Since we want to be able to shadow vars in the outer scope here
	this->symTable->PushScope();
	llvm::Function* theFunction = this->compilationUnit->builder.GetInsertBlock()->getParent();

	if (n->initStmt)
	{
		n->initStmt->accept(this);
	}

	// loopend basic block will handle updating the variable and checking for end condition
	llvm::BasicBlock* loopendbb = llvm::BasicBlock::Create(*(this->compilationUnit->context.get()), "forend", theFunction);
	// This is where a continue would go to, so set our header for continue here
	this->loopHeader = loopendbb;
	this->compilationUnit->builder.CreateBr(loopendbb);

	// the loop body is reponsible for executing the actual body of the loop, then jump
	// to loopend to increment vars and check again
	llvm::BasicBlock* loopbb = llvm::BasicBlock::Create(*(this->compilationUnit->context.get()), "forbody", theFunction);
	this->compilationUnit->builder.SetInsertPoint(loopbb);
	n->loopBody->accept(this);
	if (n->updateStmt)
	{
		n->updateStmt->accept(this);
	}
	this->compilationUnit->builder.CreateBr(loopendbb);	

	// create exit block
	llvm::BasicBlock* exitloopbb = llvm::BasicBlock::Create(*(this->compilationUnit->context.get()), "forexit", theFunction);
	// this is where a break would go to, so mark out exit
	this->loopExit = exitloopbb;

	// if we have a condition, evaluate it
	this->compilationUnit->builder.SetInsertPoint(loopendbb);
	if (n->loopCondExpr)
	{
		// evaluate loop condition
		n->loopCondExpr->accept(this);
		llvm::Value* endcondV = this->consumeRetValue();
		if (!endcondV)
		{
			// TODO: Error out here!
		}
		endcondV = this->compilationUnit->builder.CreateICmpNE(
			endcondV, 
			llvm::ConstantInt::get(llvm::Type::getInt1Ty(*(this->compilationUnit->context.get())), 0), 
			"ifcond"
		);

		this->compilationUnit->builder.CreateCondBr(endcondV, loopbb, exitloopbb);
	}
	else
	{
		// otherwise, we are just like for(...;...;)
		// so we just loop anyways!
		this->compilationUnit->builder.CreateBr(loopbb);
	}


	// we continue inserting code after the for loop
	this->compilationUnit->builder.SetInsertPoint(exitloopbb);
	this->symTable->PopScope();
	// continue generating code
}

void CodegenVisitor::visit(WhileNode* n) 
{
	// get the parent function
	llvm::Function* theFunction = this->compilationUnit->builder.GetInsertBlock()->getParent();	

	// Setup blocks needed for while loop
	llvm::BasicBlock* headerbb = llvm::BasicBlock::Create(*(this->compilationUnit->context.get()), "whileheader", theFunction);
	// continue would loop back to check condition again, so set properly
	this->loopHeader = headerbb;

	// basic block for loop body
	llvm::BasicBlock* loopbodybb = llvm::BasicBlock::Create(*(this->compilationUnit->context.get()), "whilebody", theFunction);
	
	// block for end of loop
	llvm::BasicBlock* exitloopbb = llvm::BasicBlock::Create(*(this->compilationUnit->context.get()), "whileexit", theFunction);
	// break would go to the loop end, so set properly
	this->loopExit = exitloopbb;

	// jump to our header function so we can test our condition
	this->compilationUnit->builder.CreateBr(headerbb);

	// header is where we test the condition
	this->compilationUnit->builder.SetInsertPoint(headerbb);

	// evaluate loop condition
	n->whileExpr->accept(this);
	llvm::Value* endcondV = this->consumeRetValue();
	if (!endcondV)
	{
		std::cout << "Error: Can't evaluate end condition of for loop\n";
		exit(1);
	}
	endcondV = this->compilationUnit->builder.CreateICmpNE(
		endcondV, 
		llvm::ConstantInt::get(llvm::Type::getInt1Ty(*(this->compilationUnit->context.get())), 0), 
		"whilecond"
	);
	// on true, evaluate the loop body, on false go to the end of the loop
	this->compilationUnit->builder.CreateCondBr(endcondV, loopbodybb, exitloopbb);

	// loop body is simply the body of the while statement
	this->compilationUnit->builder.SetInsertPoint(loopbodybb);
	n->loopBody->accept(this);
	// when we're done evaluating the body, jump back to the header 
	this->compilationUnit->builder.CreateBr(headerbb);	

	// set our insertion point to be after the loop exit
	this->compilationUnit->builder.SetInsertPoint(exitloopbb);
}

void CodegenVisitor::visit(UnaryNode* n) 
{
	// evaluate the sub expression
	// assume the type of the child is float or int since -bool or -void doesn't really make
	// any sense!
	// create multiplication -1 and this->retValue
	n->expr->accept(this);
	llvm::Value* val = this->consumeRetValue();

	// We only have to check the left nodes evaluated type since, due to semantic checking, left and right
	// are guaranteed to have matching types by the time we get here
	if (n->expr->evaluatedType == TypeName::tFloat)
	{
		// Floating point comparison
		this->setRetValue(this->compilationUnit->builder.CreateFMul(val, llvm::ConstantFP::get(llvm::Type::getFloatTy(*(this->compilationUnit->context.get())), -1.0f)));
	}
	else
	{
		// Integer comparison
		this->setRetValue(this->compilationUnit->builder.CreateMul(val, llvm::ConstantInt::get(llvm::Type::getInt1Ty(*(this->compilationUnit->context.get())), -1)));
	}
}

void CodegenVisitor::visit(TernaryNode* n) 
{
	// get the parent function
	llvm::Function* theFunction = this->compilationUnit->builder.GetInsertBlock()->getParent();	

	// first we'll evaluate the if condition
	n->condExpr->accept(this);
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

	// setup basic blocks needed for ternary operation
	llvm::BasicBlock* trueBB = llvm::BasicBlock::Create(*(this->compilationUnit->context.get()), "trueval", theFunction);
	llvm::BasicBlock* falseBB = llvm::BasicBlock::Create(*(this->compilationUnit->context.get()), "falseval");
	llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*(this->compilationUnit->context.get()), "mergeval");

	// jump based on condition
	this->compilationUnit->builder.CreateCondBr(condV, trueBB, falseBB);

	// true basic block
	this->compilationUnit->builder.SetInsertPoint(trueBB);
	n->trueExpr->accept(this);
	llvm::Value* trueV = this->consumeRetValue();
	this->compilationUnit->builder.CreateBr(mergeBB);
	trueBB = this->compilationUnit->builder.GetInsertBlock();

	// false basic block
	theFunction->getBasicBlockList().push_back(falseBB);
	this->compilationUnit->builder.SetInsertPoint(falseBB);
	n->falseExpr->accept(this);
	llvm::Value* falseV = this->consumeRetValue();
	this->compilationUnit->builder.CreateBr(mergeBB);
	//trueBB = this->compilationUnit->builder.GetInsertBlock();

	// use phi to choose between two above values
	theFunction->getBasicBlockList().push_back(mergeBB);
	this->compilationUnit->builder.SetInsertPoint(mergeBB);
	llvm::Type* t = this->GetLLVMType(n->evaluatedType);
	llvm::PHINode* PN = this->compilationUnit->builder.CreatePHI(t, 2, "ternary");
	PN->addIncoming(trueV, trueBB);
	PN->addIncoming(falseV, falseBB);

	this->setRetValue(PN);
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
	// break out a loop by jumping to the after label
	// TODO: What about nested breaks/continues? We need a STACK of headers/exits?
	this->compilationUnit->builder.CreateBr(this->loopExit);
}

void CodegenVisitor::visit(ContinueNode* n) 
{
	// jump straight back to the current header label
	// TODO: what about nested breaks/continues? We need a STACK of headers/exits?
	this->compilationUnit->builder.CreateBr(this->loopHeader);
}	

// Helper functions for our types -> llvm types
// TODO: Move these out of here!

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

llvm::Value* CodegenVisitor::GetLLVMAugmentedAssignOpsInt(AugmentedAssignOps a, llvm::Value* lhs, llvm::Value* rhs)
{
	// translate from BinaryOp enums used in AST/semantic analysis into llvms native functions
	switch(a)
	{
		case AugmentedAssignOps::PlusEq:
			return this->compilationUnit->builder.CreateAdd(lhs, rhs);
		case AugmentedAssignOps::MinusEq:
			return this->compilationUnit->builder.CreateSub(lhs, rhs);
		case AugmentedAssignOps::StarEq:
			return this->compilationUnit->builder.CreateMul(lhs, rhs);
		case AugmentedAssignOps::SlashEq:
			return this->compilationUnit->builder.CreateSDiv(lhs, rhs);
		default:
			llvm_unreachable("Invalid binary operator");
			return nullptr;
	}
}

llvm::Value* CodegenVisitor::GetLLVMAugmentedAssignOpsFP(AugmentedAssignOps a, llvm::Value* lhs, llvm::Value* rhs)
{
	// translate from BinaryOp enums used in AST/semantic analysis into llvms native functions
	switch(a) 
	{
		case AugmentedAssignOps::PlusEq:
			return this->compilationUnit->builder.CreateFAdd(lhs, rhs);
		case AugmentedAssignOps::MinusEq:
			return this->compilationUnit->builder.CreateFSub(lhs, rhs);
		case AugmentedAssignOps::StarEq:
			return this->compilationUnit->builder.CreateFMul(lhs, rhs);
		case AugmentedAssignOps::SlashEq:
			return this->compilationUnit->builder.CreateFDiv(lhs, rhs);
		default:
			llvm_unreachable("Invalid binary operator");
			return nullptr;
	}
}

llvm::AllocaInst* CodegenVisitor::CreateEntryBlockAlloca(llvm::Function* TheFunction, std::string VarName, llvm::Type* t)
{
	// Alloca space for a variable in the entry block of our current function
	llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
		TheFunction->getEntryBlock().begin());
	return TmpB.CreateAlloca(t, 0, VarName);
}
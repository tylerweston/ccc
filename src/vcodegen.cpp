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

// Error generating
#include "llvm/Support/Error.h"

CodegenVisitor::CodegenVisitor()
{
	// pass
}

void CodegenVisitor::visit(VariableNode* n) 
{

	// std::string name;

	//this->compilationUnit->builder
	//this->compilationUnit->
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

	// enum class BinaryOps
	// {
	// 	Plus,
	// 	Minus,
	// 	Star,
	// 	Slash,
	// 	LogAnd,
	// 	LogOr
	// };
}

void CodegenVisitor::visit(LogicalOpNode* n) 
{
	// binary, logical, and relational op are all basically the same thing
	// BinaryOps op;
	// std::unique_ptr<ExpressionNode> left;
	// std::unique_ptr<ExpressionNode> right;

	// 	LogAnd,
	// 	LogOr
}

void CodegenVisitor::visit(RelationalOpNode* n) 
{	
	// binary, logical, and relational op are all basically the same thing

	// RelationalOps op;
	// std::unique_ptr<ExpressionNode> left;
	// std::unique_ptr<ExpressionNode> right;

	// enum class RelationalOps
	// {
	// 	Eq, 
	// 	Ne, 
	// 	Lt,
	// 	Gt,
	// 	Le, 
	// 	Ge
	// };
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

	// A block means we have to generate a new scope!
}

void CodegenVisitor::visit(FuncDefnNode* n) 
{

	// std::unique_ptr<Node> funcDecl;
	// std::unique_ptr<Node> funcBody;

	// Here, we start creating actual blocks and inserting them into the module via the builder (?!)
}

void CodegenVisitor::visit(FuncDeclNode* n) 
{
	// Generate our parameter LLVM types from our AST parameter types
	std::vector<llvm::Type*> parameters;
	for (int i = 0; i < (int) n->params.size(); i++) 
	{
		parameters.push_back(GetLLVMType(
			n->params[i]->t,
			&(this->compilationUnit->builder)
		));
	}

	// Create our LLVM function signature
	llvm::FunctionType* signature = llvm::FunctionType::get(
		GetLLVMType(n->t, &(this->compilationUnit->builder)) /*return type*/,
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

	// LAB CODE:

	// std::vector<llvm::Type*> parameters;
	// llvm::FunctionType* signature = llvm::FunctionType::get(return_type, parameters, /* isVarArg */ false);
	// llvm::Function* f = llvm::Function::Create(signature, llvm::Function::ExternalLinkage, name, module);
	// size_t i = 0;
	// for (llvm::Argument& a : f->args()) {
	// 	a.setName(name);	
	// }

	// FuncDeclNode:
	//   std::vector<std::unique_ptr<DeclarationNode>> params;
	//   TypeName t;

	// DeclarationNode:
	//   std::string name;
	//   TypeName t;

	// Tutorial Code:

	// 	Function *PrototypeAST::codegen() {
	//   // Make the function type:  double(double,double) etc.
	//   std::vector<Type*> Doubles(Args.size(), Type::getDoubleTy(TheContext));
	//   FunctionType *FT = FunctionType::get(Type::getDoubleTy(TheContext), Doubles, false);
	//   Function *F = Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());
}

void CodegenVisitor::visit(FuncCallNode* n) 
{
	// std::string name;
	// std::vector<std::unique_ptr<ExpressionNode>> funcArgs;

	// check out how to call functions here, we need to visit the insertion point set in the function table?
}

void CodegenVisitor::visit(ConstantIntNode* n) 
{
	// llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), value, signed);
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
}

void CodegenVisitor::visit(ReturnNode* n) 
{
	// How does this work
}

void CodegenVisitor::visit(ConstantFloatNode* n) 
{
	// llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), value);
}

void CodegenVisitor::visit(IfNode* n) 
{
	// this->ifExpr = std::move(ifExpr);
	// this->ifBody = std::move(ifBody);

	// basic block, compare and jump on comparison success or fail?

	// condition:
	// evaluate if condition
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
	// install value of expression, but negativified
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
}

void CodegenVisitor::visit(ExpressionStatementNode* n)
{
	// this->expr = std::move(expr); 	// he expression to be evaluated

	// evaluate the subexpression (in case of side effects)
	// but then we can just toss the results!
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

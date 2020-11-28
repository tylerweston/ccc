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
	// Setup things we need to generate IR
	// static llvm::LLVMContext TheContext;						// Holds core LLVM info
	// static llvm::IRBuilder<> Builder(TheContext);				// Helper function to generate LLVM instructions
	// static std::unique_ptr<llvm::Module> TheModule;				// Contains functions and global variables
	// static std::map<std::string, llvm::Value *> NamedValues;	// Keeps track of values that are defined in the current scope
}

void CodegenVisitor::visit(VariableNode* n) 
{
	//this->compilationUnit->builder
	//this->compilationUnit->
}

void CodegenVisitor::visit(DeclarationNode* n) 
{
	// llvm::Type::getIntNTy(context, num_bits)	// Create integer type
}

void CodegenVisitor::visit(DeclAndAssignNode* n) 
{
}

void CodegenVisitor::visit(BinaryOpNode* n) 
{
}

void CodegenVisitor::visit(LogicalOpNode* n) 
{
}

void CodegenVisitor::visit(RelationalOpNode* n) 
{
}

void CodegenVisitor::visit(RootNode* n) 
{
}

void CodegenVisitor::visit(BlockNode* n) 
{
}

void CodegenVisitor::visit(FuncDefnNode* n) 
{
	// Here, we start creating actual blocks and inserting them into the module via the builder (?!)
}

void CodegenVisitor::visit(FuncDeclNode* n) 
{
	// Attempt1:

	// Generate our parameter LLVM types from our AST parameter types
	std::vector<llvm::Type*> parameters;
	// for (auto decl : n->params)
	// {	
	// 	parameters.push_back(GetLLVMType(decl->t, this->builder*));
	// }
	for (int i = 0; i < (int) n->params.size(); i++) {
		parameters.push_back(GetLLVMType(
			n->params[i].get(),
			this->compilationUnit->builder*
		);
	}

	// Create our LLVM function signature
	llvm::FunctionType* signature = llvm::FunctionType::get(
		GetLLVMType(n->t, this->builder*) /*return type*/,
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
	// generate parameter  and function name iterator
	auto paramNameIter = n->params.begin();
	auto funcNameIter = f->args().begin();
	// loop over lists in parallel and name function args with their appropriate names
	while (paramNameIter != n->params.end())
	{
		// oooof? what are the odds this works
		(*funcNameIter)->setName((*paramNameIter)->name);
		funcNameIter++;
		paramNameIter++;
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
}

void CodegenVisitor::visit(ConstantIntNode* n) 
{
	// llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), value, signed);
}

void CodegenVisitor::visit(AssignmentNode* n) 
{
}

void CodegenVisitor::visit(AugmentedAssignmentNode* n) 
{
}

void CodegenVisitor::visit(BoolNode* n) 
{
}

void CodegenVisitor::visit(ReturnNode* n) 
{
}

void CodegenVisitor::visit(ConstantFloatNode* n) 
{
	// llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), value);
}

void CodegenVisitor::visit(IfNode* n) 
{
}

void CodegenVisitor::visit(ForNode* n) 
{
}

void CodegenVisitor::visit(WhileNode* n) 
{
}

void CodegenVisitor::visit(UnaryNode* n) 
{
}

void CodegenVisitor::visit(TernaryNode* n) 
{
}

void CodegenVisitor::visit(CastExpressionNode* n) 
{
}

void CodegenVisitor::visit(ExpressionStatementNode* n)
{
}

void CodegenVisitor::visit(BreakNode* n) 
{
}

void CodegenVisitor::visit(ContinueNode* n) 
{
}	

void CodegenVisitor::indent()
{
}

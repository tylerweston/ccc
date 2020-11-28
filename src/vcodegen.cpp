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

CodegenVisitor::CodegenVisitor()
{
	// Setup things we need to generate IR
	static llvm::LLVMContext TheContext;
	static llvm::IRBuilder<> Builder(TheContext);
	static std::unique_ptr<llvm::Module> TheModule;
	static std::map<std::string, llvm::Value *> NamedValues;
}

void CodegenVisitor::visit(VariableNode* n) 
{
}

void CodegenVisitor::visit(DeclarationNode* n) 
{
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
}

void CodegenVisitor::visit(FuncDeclNode* n) 
{
}

void CodegenVisitor::visit(FuncCallNode* n) 
{
}

void CodegenVisitor::visit(ConstantIntNode* n) 
{
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

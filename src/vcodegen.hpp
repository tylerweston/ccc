/*
	CodeGen
*/
#ifndef ECE467_CODEGEN_HPP_INCLUDED
#define ECE467_CODEGEN_HPP_INCLUDED

#include <memory>
#include <cstdio>
#include "common.hpp"
#include "nodes.hpp"
#include "compiler.hpp"
// #include "llvm/IR/IRBuilder.h"
// #include "llvm/IR/LLVMContext.h"
// #include "llvm/IR/Module.h"
// #include "llvm/IR/Type.h"
#include "llvm/IR/ValueHandle.h"


class CodegenVisitor : public NodeVisitor
{
private:
	llvm::Value* retValue;
public:
	CompilationUnit* compilationUnit;
	// Includes necessary to build IR
	// will this live here and get init'ed somewhere else
	CodegenVisitor();
	llvm::Value* consumeRetValue();
	void setRetValue(llvm::Value* v);
	void visit(VariableNode* n) override;
	void visit(DeclarationNode* n) override;
	void visit(DeclAndAssignNode* n) override;
	void visit(BinaryOpNode* n) override;
	void visit(LogicalOpNode* n) override;
	void visit(RelationalOpNode* n) override;
	void visit(RootNode* n) override;
	void visit(BlockNode* n) override;
	void visit(FuncDefnNode* n) override;
	void visit(FuncDeclNode* n) override;
	void visit(FuncCallNode* n) override;
	void visit(ConstantIntNode* n) override;
	void visit(AssignmentNode* n) override;
	void visit(AugmentedAssignmentNode* n) override;
	void visit(BoolNode* n) override;
	void visit(ReturnNode* n) override;
	void visit(ConstantFloatNode* n) override;
	void visit(IfNode* n) override;
	void visit(ForNode* n) override;
	void visit(WhileNode* n) override;
	void visit(UnaryNode* n) override;
	void visit(TernaryNode* n) override;
	void visit(CastExpressionNode* n) override;
	void visit(BreakNode* n) override;
	void visit(ContinueNode* n) override;	
	void visit(ExpressionStatementNode*) override;

	llvm::Type* GetLLVMType(TypeName t);
	llvm::Value* GetLLVMBinaryOp(BinaryOps b, llvm::Value* lhs, llvm::Value* rhs);
	llvm::Value* GetLLVMRelationalOp(RelationalOps r, llvm::Value* lhs, llvm::Value* rhs);
};

#endif // ECE467_CODEGEN_HPP_INCLUDED

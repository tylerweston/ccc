/*
	CodeGen
*/
#ifndef CCC_CODEGEN_HPP_INCLUDED
#define CCC_CODEGEN_HPP_INCLUDED

#include <memory>
#include <cstdio>
#include <map>
#include <vector>
#include "common.hpp"
#include "nodes.hpp"
#include "compiler.hpp"
#include "symtable.hpp"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/ValueHandle.h"


class CodegenVisitor : public NodeVisitor
{
private:
	llvm::Value* retValue;
	bool returnFlag;

	SymbolTable* symTable;

	llvm::Value* consumeRetValue();
	std::vector<llvm::BasicBlock*> loopHeaders;
	std::vector<llvm::BasicBlock*> loopFooters;
	std::vector<llvm::BasicBlock*> loopExits;

	llvm::Type* GetLLVMType(TypeName t);
	llvm::Value* GetLLVMBinaryOpInt(BinaryOps b, llvm::Value* lhs, llvm::Value* rhs);
	llvm::Value* GetLLVMBinaryOpFP(BinaryOps b, llvm::Value* lhs, llvm::Value* rhs);
	llvm::Value* GetLLVMRelationalOpInt(RelationalOps r, llvm::Value* lhs, llvm::Value* rhs);
	llvm::Value* GetLLVMRelationalOpFP(RelationalOps r, llvm::Value* lhs, llvm::Value* rhs);
	llvm::Value* GetLLVMAugmentedAssignOpsInt(AugmentedAssignOps a, llvm::Value* lhs, llvm::Value* rhs);
	llvm::Value* GetLLVMAugmentedAssignOpsFP(AugmentedAssignOps a, llvm::Value* lhs, llvm::Value* rhs);

	llvm::AllocaInst* CreateEntryBlockAlloca(llvm::Function* TheFunction, std::string VarName, llvm::Type* t);


public:
	CompilationUnit* compilationUnit;
	// Includes necessary to build IR
	// will this live here and get init'ed somewhere else
	CodegenVisitor();
	~CodegenVisitor();
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
	void visit(AssignmentNode* n) override;
	void visit(AugmentedAssignmentNode* n) override;
	void visit(ReturnNode* n) override;
	void visit(ConstantBoolNode* n) override;
	void visit(ConstantCharNode* n) override;
	void visit(ConstantDoubleNode* n) override;
	void visit(ConstantFloatNode* n) override;
	void visit(ConstantIntNode* n) override;
	void visit(IfNode* n) override;
	void visit(ForNode* n) override;
	void visit(WhileNode* n) override;
	void visit(UnaryNode* n) override;
	void visit(TernaryNode* n) override;
	void visit(CastExpressionNode* n) override;
	void visit(BreakNode* n) override;
	void visit(ContinueNode* n) override;	
	void visit(ExpressionStatementNode*) override;
};

#endif // CCC_CODEGEN_HPP_INCLUDED

/*
	EvaluateVisitor
*/
#ifndef ECE467_EVALUATE_HPP_INCLUDED
#define ECE467_EVALUATE_HPP_INCLUDED

#include <memory>
#include <cstdio>
#include "common.hpp"
#include "nodes.hpp"
#include "symtable.hpp"

/*
Visit nodes and fill in some information about the AST to use in the semantic analysis phase.
Namely, we begin filling in the symbol table, function table, and evaluating expressions.
*/

class EvaluateVisitor : public NodeVisitor
{
public:
	SymbolTable* symbolTable;
	FunctionTable* functionTable;
	bool needReturn;
	bool hasReturn;
	bool inFuncDef = false;

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
};

#endif // ECE467_PRINTER_HPP_INCLUDED

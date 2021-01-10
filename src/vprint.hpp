/*
	PrintVisitor
*/
#ifndef CCC_PRINTER_HPP_INCLUDED
#define CCC_PRINTER_HPP_INCLUDED

#include <memory>
#include <cstdio>
#include "common.hpp"
#include "nodes.hpp"

class PrintVisitor : public NodeVisitor
{
public:
	int indent_level = 0;
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
	void indent();
};

#endif // CCC_PRINTER_HPP_INCLUDED

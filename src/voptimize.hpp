/*
	PrintVisitor
*/
#ifndef CCC_OPTIMIZE_HPP_INCLUDED
#define CCC_OPTIMIZE_HPP_INCLUDED

#include <memory>
#include <cstdio>
#include "common.hpp"
#include "nodes.hpp"

class OptimizeVisitor : public NodeVisitor
{
public:
	OptimizeVisitor();
	bool cleanTree;							// once we've looped through the tree without changing anything, we're done
	std::unique_ptr<Node> replacement_node;	// if we're going to replace a node, hold it here
	std::unique_ptr<ExpressionNode> repl_expr_node;
	std::vector<std::unique_ptr<Node>> node_list;	// holds our if statement 
	bool insertNodeVector;							// do we wanna insert our if body
	bool hasReplacement;					// flag for if we have a replacement node or not
	bool removeNode;						// marks a node for removal

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

#endif // CCC_PRINTER_HPP_INCLUDED

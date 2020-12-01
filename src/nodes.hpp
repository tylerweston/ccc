#ifndef ECE467_NODE_HPP_INCLUDED
#define ECE467_NODE_HPP_INCLUDED

#include "location.hh"
#include "common.hpp"
#include <memory>
#include <map>
#include <vector>

/*
	TODO:
	- name and type are in different order in different nodes
	- what feels better? things are declared like type name, but name is more important?
	- ControlFlow node that we can sub-class continue and break from
*/

// Forward declare NodeVisitor
class NodeVisitor;

// Base Node class
class Node 
{
public:
	yy::location location;		// if there is an error, we want to display our location, location->begin.line, location->begin.column
								// JUST REPORT THE FIRST ERROR YOU FIND!

	virtual ~Node() = 0;	// we'll call our children's destructors!
							// = 0 means it's undefined, so we can't allocate a class of type Node directly!
							// = 0 means a child class MUST implement it!
	virtual void accept(NodeVisitor*) = 0;	// visitor?? the = 0 means a child MUST implement it
};

class ExpressionNode : public Node
{
	// This will be the parent of all expression nodes!
	// this has a value that it evaluates to, but it will be a different value depending
	// on the type of it's children?
	// Expressions EVALUATE to something
	// For now, we just care about the TYPE and if this node is CONSTANT
public:
	TypeName evaluatedType;
	bool isConstant;
};

class ConstantNode
{
public:
	union
	{
		// An expression will evaluate to one of these types
		// we can use TypeName to decide what it evaluates to.
		int intValue;
		float floatValue;
		bool boolValue;
	};
};

class StatementNode : public Node
{
	// this will be the parent of all our statement nodes
	// Statements DO NOT EVALUATE to anything, they just E X I S T
};

class ExpressionStatementNode : public StatementNode
{
public:
	std::unique_ptr<ExpressionNode> expr;	
	ExpressionStatementNode(std::unique_ptr<ExpressionNode> expr);
	virtual void accept(NodeVisitor* v) override;
};

class VariableNode : public ExpressionNode
{
public:
	std::string name;
	VariableNode(std::string in);
	virtual void accept(NodeVisitor* v) override;
};

class DeclarationNode : public StatementNode
{
public:
	std::string name;
	TypeName t;
	DeclarationNode(TypeName t, std::string name);
	virtual void accept(NodeVisitor* v) override;
};

class DeclAndAssignNode : public StatementNode
{
public:	
	std::unique_ptr<DeclarationNode> decl;
	std::unique_ptr<ExpressionNode> expr;
	DeclAndAssignNode(std::unique_ptr<DeclarationNode> decl, std::unique_ptr<ExpressionNode> expr);
	virtual void accept(NodeVisitor* v) override;
};

class BinaryOpNode : public ExpressionNode 
{
public:
	BinaryOps op;
	std::unique_ptr<ExpressionNode> left;
	std::unique_ptr<ExpressionNode> right;
	BinaryOpNode(BinaryOps operation, std::unique_ptr<ExpressionNode> left, std::unique_ptr<ExpressionNode> right);
	virtual void accept(NodeVisitor* v) override;
};

class RelationalOpNode : public ExpressionNode 
{
public:
	RelationalOps op;
	std::unique_ptr<ExpressionNode> left;
	std::unique_ptr<ExpressionNode> right;
	RelationalOpNode(RelationalOps operation, std::unique_ptr<ExpressionNode> left, std::unique_ptr<ExpressionNode> right);
	virtual void accept(NodeVisitor* v) override;
};

class LogicalOpNode : public ExpressionNode
{
public:
	BinaryOps op;
	std::unique_ptr<ExpressionNode> left;
	std::unique_ptr<ExpressionNode> right;
	LogicalOpNode(BinaryOps operation, std::unique_ptr<ExpressionNode> left, std::unique_ptr<ExpressionNode> right);
	virtual void accept(NodeVisitor* v) override;	
};

class RootNode : public Node
{
public:	
	std::vector<std::unique_ptr<Node>> funcs;
	RootNode(std::vector<std::unique_ptr<Node>> funcs);
	virtual void accept(NodeVisitor* v) override;
};

class BlockNode : public Node
{
public:
	std::vector<std::unique_ptr<Node>> stmts;
	BlockNode(std::vector<std::unique_ptr<Node>> stmts);
	virtual void accept(NodeVisitor* v) override;
};

class FuncDefnNode : public Node
{
public:
	std::unique_ptr<FuncDeclNode> funcDecl;
	std::unique_ptr<Node> funcBody;
	FuncDefnNode(std::unique_ptr<FuncDeclNode> funcDecl, std::unique_ptr<Node> funcBody);
	virtual void accept(NodeVisitor* v) override;
};

class FuncDeclNode : public Node 
{
public:
	std::string name;
	TypeName t;
	std::vector<std::unique_ptr<DeclarationNode>> params;
	FuncDeclNode(TypeName t, std::string name, std::vector<std::unique_ptr<DeclarationNode>> params);
	virtual void accept(NodeVisitor* v) override;
};

class FuncCallNode : public ExpressionNode
{
public:
	std::string name;
	std::vector<std::unique_ptr<ExpressionNode>> funcArgs;
	FuncCallNode(std::string name, std::vector<std::unique_ptr<ExpressionNode>> funcArgs);
	virtual void accept(NodeVisitor* v) override;
};

class AssignmentNode : public StatementNode
{
public:
	std::string name;	// can be name or declaration
	std::unique_ptr<ExpressionNode> expr;	// the expression we are going to assign to name
	AssignmentNode(std::string name, std::unique_ptr<ExpressionNode> expr);
	virtual void accept(NodeVisitor* v) override;
};

class AugmentedAssignmentNode : public StatementNode
{
public:
	AugmentedAssignOps op;
	std::string name;
	std::unique_ptr<ExpressionNode> expr;	// the expression we are going to assign to name
	AugmentedAssignmentNode(AugmentedAssignOps op, std::string name, std::unique_ptr<ExpressionNode> expr);
	virtual void accept(NodeVisitor* v) override;
};

class BoolNode : public ExpressionNode, public ConstantNode
{
public:
	//bool value;
	BoolNode(bool in);
	virtual void accept(NodeVisitor* v) override;
};

class ConstantIntNode : public ExpressionNode, public ConstantNode
{
public:
	//int value;
	ConstantIntNode(int in);
	virtual void accept(NodeVisitor* v) override;
};

class ConstantFloatNode : public ExpressionNode, public ConstantNode
{
public:
	//float value;
	ConstantFloatNode(float in);
	virtual void accept(NodeVisitor* v) override;
};

class IfNode : public StatementNode
{
public:
	std::unique_ptr<ExpressionNode> ifExpr;
	std::unique_ptr<Node> ifBody;
	IfNode(std::unique_ptr<ExpressionNode> ifExpr, std::unique_ptr<Node> ifBody);
	virtual void accept(NodeVisitor* v) override;
};

class ForNode : public StatementNode
{
public:
	std::unique_ptr<Node> initStmt;
	std::unique_ptr<ExpressionNode> loopCondExpr;
	std::unique_ptr<Node> updateStmt;
	std::unique_ptr<Node> loopBody; 
	ForNode(std::unique_ptr<Node> initStmt, std::unique_ptr<ExpressionNode> midExpr, std::unique_ptr<Node> loopCondStmt, std::unique_ptr<Node> loopBody);
	virtual void accept(NodeVisitor* v) override;
};

class WhileNode : public StatementNode
{
public:
	std::unique_ptr<ExpressionNode> whileExpr;
	std::unique_ptr<Node> loopBody;
	WhileNode(std::unique_ptr<ExpressionNode> whileExpr, std::unique_ptr<Node> loopBody);
	virtual void accept(NodeVisitor* v) override;
};

class UnaryNode : public ExpressionNode
{
public:
	std::unique_ptr<ExpressionNode> expr;
	UnaryNode(std::unique_ptr<ExpressionNode> expr);
	virtual void accept(NodeVisitor* v) override;
};

class TernaryNode : public ExpressionNode
{
public:
	std::unique_ptr<ExpressionNode> condExpr;
	std::unique_ptr<ExpressionNode> trueExpr;
	std::unique_ptr<ExpressionNode> falseExpr;
	TernaryNode(std::unique_ptr<ExpressionNode> condExpr, std::unique_ptr<ExpressionNode> trueExpr, std::unique_ptr<ExpressionNode> falseExpr);
	virtual void accept(NodeVisitor* v) override;
};

class CastExpressionNode : public ExpressionNode
{
public:
	TypeName t;
	std::unique_ptr<ExpressionNode> expr;
	CastExpressionNode(TypeName t, std::unique_ptr<ExpressionNode> expr);
	virtual void accept(NodeVisitor* v) override;
};

class ReturnNode : public StatementNode
{
public:
	std::unique_ptr<ExpressionNode> expr;
	ReturnNode(std::unique_ptr<ExpressionNode> expr);
	virtual void accept(NodeVisitor* v) override;
};

class BreakNode : public StatementNode
{
public:
	BreakNode();
	virtual void accept(NodeVisitor* v) override;
};

class ContinueNode : public StatementNode
{
public:
	ContinueNode();
	virtual void accept(NodeVisitor* v) override;
};

#endif // ECE467_NODE_HPP_INCLUDED
													
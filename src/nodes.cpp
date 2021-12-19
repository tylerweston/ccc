#include "nodes.hpp"

Node::~Node() = default;

RootNode::RootNode(std::vector<std::unique_ptr<Node>> funcs) { this->funcs = std::move(funcs); }
void RootNode::accept(NodeVisitor* v) { v->visit(this); }

ConstantIntNode::ConstantIntNode(int in) 
{ 
	this->intValue = in; 
	this->isConstant = true; 
	this->evaluatedType = TypeName::tInt;
} 
void ConstantIntNode::accept(NodeVisitor* v) { v->visit(this); }

ConstantFloatNode::ConstantFloatNode(float in) 
{ 
	this->floatValue = in; 
	this->isConstant = true; 
	this->evaluatedType = TypeName::tFloat;
}
void ConstantFloatNode::accept(NodeVisitor* v) { v->visit(this); }

ConstantBoolNode::ConstantBoolNode(bool in) 
{ 
	this->boolValue = in; 
	this->isConstant = true; 
	this->evaluatedType = TypeName::tBool;
}
void ConstantBoolNode::accept(NodeVisitor* v) { v->visit(this); }

ConstantCharNode::ConstantCharNode(char in)
{
	this->charValue = in;
	this->isConstant = true;
	this->evaluatedType = TypeName::tChar;
}
void ConstantCharNode::accept(NodeVisitor* v) { v->visit(this); }

ConstantDoubleNode::ConstantDoubleNode(double in)
{
	this->doubleValue = in;
	this->isConstant = true;
	this->evaluatedType = TypeName::tChar;
}
void ConstantDoubleNode::accept(NodeVisitor* v) { v->visit(this); }

VariableNode::VariableNode(std::string in): name(in) {}
void VariableNode::accept(NodeVisitor* v) { v->visit(this); }

BreakNode::BreakNode() {;}
void BreakNode::accept(NodeVisitor* v) { v->visit(this); }

ContinueNode::ContinueNode() {;}
void ContinueNode::accept(NodeVisitor* v) { v->visit(this); }


ReturnNode::ReturnNode(std::unique_ptr<ExpressionNode> expr) 
{ 
	this->expr = std::move(expr); 
}
void ReturnNode::accept(NodeVisitor* v) { v->visit(this); }

FuncDeclNode::FuncDeclNode(TypeName t, std::string name, std::vector<std::unique_ptr<DeclarationNode>> params)
{
  this->t = t;
  this->name = std::move(name); 
  this->params = std::move(params);
}
void FuncDeclNode::accept(NodeVisitor* v) { v->visit(this); }

FuncDefnNode::FuncDefnNode(std::unique_ptr<FuncDeclNode> funcDecl, std::unique_ptr<Node> funcBody)
{
	this->funcDecl = std::move(funcDecl);
	this->funcBody = std::move(funcBody);
}
void FuncDefnNode::accept(NodeVisitor* v) { v->visit(this); }

FuncCallNode::FuncCallNode(std::string name, std::vector<std::unique_ptr<ExpressionNode>> funcArgs)
{
	this->name = std::move(name);
	this->funcArgs = std::move(funcArgs);
}
void FuncCallNode::accept(NodeVisitor* v) { v->visit(this); }

DeclarationNode::DeclarationNode(TypeName t, std::string name, bool isConstant)
{
  this->t = t;
  this->name = std::move(name); 
  this->isConstant = isConstant;
}
void DeclarationNode::accept(NodeVisitor* v) { v->visit(this); }

DeclAndAssignNode::DeclAndAssignNode(std::unique_ptr<DeclarationNode> decl, std::unique_ptr<ExpressionNode> expr)
{
  this->decl = std::move(decl);
  this->expr = std::move(expr);	
}
void DeclAndAssignNode::accept(NodeVisitor* v) { v->visit(this); }

BinaryOpNode::BinaryOpNode(BinaryOps op, std::unique_ptr<ExpressionNode> left, std::unique_ptr<ExpressionNode> right)
{
	this->op = op;
	this->left = std::move(left); 	// might be able to add this to parser.y so $1 -> std::move($1)
	this->right = std::move(right);
}
void BinaryOpNode::accept(NodeVisitor* v) { v->visit(this); }

RelationalOpNode::RelationalOpNode(RelationalOps op, std::unique_ptr<ExpressionNode> left, std::unique_ptr<ExpressionNode> right)
{
	this->op = op;
	this->left = std::move(left);
	this->right = std::move(right);
}
void RelationalOpNode::accept(NodeVisitor* v) { v->visit(this); }

LogicalOpNode::LogicalOpNode(BinaryOps op, std::unique_ptr<ExpressionNode> left, std::unique_ptr<ExpressionNode> right)
{
	this->op = op;
	this->left = std::move(left); 	// might be able to add this to parser.y so $1 -> std::move($1)
	this->right = std::move(right);
}
void LogicalOpNode::accept(NodeVisitor* v) { v->visit(this); }

AssignmentNode::AssignmentNode(std::string name, std::unique_ptr<ExpressionNode> expr)
{
	this->name = std::move(name);
	this->expr = std::move(expr);
}
void AssignmentNode::accept(NodeVisitor* v) { v->visit(this); }

AugmentedAssignmentNode::AugmentedAssignmentNode(AugmentedAssignOps op, std::string name, std::unique_ptr<ExpressionNode> expr)
{
	this->op = op;
	this->name = std::move(name);
	this->expr = std::move(expr);
}
void AugmentedAssignmentNode::accept(NodeVisitor* v) { v->visit(this); }

IfNode::IfNode(std::unique_ptr<ExpressionNode> ifExpr, std::unique_ptr<Node> ifBody)
{
	this->ifExpr = std::move(ifExpr);
	this->ifBody = std::move(ifBody);
}
void IfNode::accept(NodeVisitor* v) { v->visit(this); }

ForNode::ForNode(std::unique_ptr<Node> initStmt, std::unique_ptr<ExpressionNode> loopCondExpr, std::unique_ptr<Node> updateStmt, std::unique_ptr<Node> loopBody)
{
	this->initStmt = std::move(initStmt);
	this->loopCondExpr = std::move(loopCondExpr);
	this->updateStmt = std::move(updateStmt);

	this->loopBody = std::move(loopBody);
}
void ForNode::accept(NodeVisitor* v) { v->visit(this); }

WhileNode::WhileNode(std::unique_ptr<ExpressionNode> whileExpr, std::unique_ptr<Node> loopBody)
{
	this->whileExpr = std::move(whileExpr);
	this->loopBody = std::move(loopBody);
}
void WhileNode::accept(NodeVisitor* v) { v->visit(this); }

TernaryNode::TernaryNode(std::unique_ptr<ExpressionNode> condExpr, std::unique_ptr<ExpressionNode> trueExpr, std::unique_ptr<ExpressionNode> falseExpr)
{
	this->condExpr = std::move(condExpr);
	this->trueExpr = std::move(trueExpr);
	this->falseExpr = std::move(falseExpr);
}
void TernaryNode::accept(NodeVisitor* v) { v->visit(this); }

UnaryNode::UnaryNode(UnaryOps op, std::unique_ptr<ExpressionNode> expr)
{
	this->expr = std::move(expr);
	this->op = op;
}
void UnaryNode::accept(NodeVisitor* v) { v->visit(this); }

BlockNode::BlockNode(std::vector<std::unique_ptr<Node>> stmts)
{
	this->stmts = std::move(stmts);
}
void BlockNode::accept(NodeVisitor* v) { v->visit(this); }

CastExpressionNode::CastExpressionNode(TypeName t, std::unique_ptr<ExpressionNode> expr)
{
	this->t = t;
	this->expr = std::move(expr);
}
void CastExpressionNode::accept(NodeVisitor* v) { v->visit(this); }

ExpressionStatementNode::ExpressionStatementNode(std::unique_ptr<ExpressionNode> expr)
{
	this->expr = std::move(expr);
}
void ExpressionStatementNode::accept(NodeVisitor* v) { v->visit(this); }

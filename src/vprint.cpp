#include "vprint.hpp"
#include "nodes.hpp"
#include "common.hpp"
#include <memory>
#include <iostream>
#include <string>

// todo: yuck, is there a better way to deal with indenting in this? maybe wrap std::cout in like an INDENT_PRINT macro??

void PrintVisitor::visit(VariableNode* n) 
{
	this->indent();
	std::cout << "Variable (" << n->location.begin.line << ", " << n->location.begin.column << ") { " << n->name << " }\n";
}

void PrintVisitor::visit(DeclarationNode* n) 
{
	this->indent();
	std::cout << "Declaration (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n"; 
	this->indent_level++;
	this->indent();
	std::cout << "Type: " << TypeNameString(n->t) << "\n"; 
	this->indent();
	std::cout << "Name: "<< n->name << "\n";
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(DeclAndAssignNode* n) 
{
	this->indent();
	std::cout << "DeclAndAssign (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	this->indent_level++;
	n->decl->accept(this);
	this->indent();
	std::cout << "=\n";
	n->expr->accept(this);
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(BinaryOpNode* n) 
{
	this->indent();
	std::cout << "BinaryOp (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	this->indent_level++;
	n->left->accept(this);
	this->indent();
	std::cout << BinaryOpString(n->op) << "\n";
	n->right->accept(this);
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(LogicalOpNode* n) 
{
	this->indent();
	std::cout << "LogicalOp (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	this->indent_level++;
	n->left->accept(this);
	this->indent();
	std::cout << BinaryOpString(n->op) << "\n";
	n->right->accept(this);
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(RelationalOpNode* n) 
{
	this->indent();
	std::cout << "RelationalyOp (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	this->indent_level++;
	n->left->accept(this);
	this->indent();
	std::cout << RelationalOpsString(n->op) << "\n";
	n->right->accept(this);
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(RootNode* n) 
{
	std::cout << "RootNode (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	this->indent_level++;
	for (auto& func : n->funcs)
	{
		func->accept(this);
	}
	this->indent_level--;
	std::cout << "}\n";
}

void PrintVisitor::visit(BlockNode* n) 
{
	this->indent();
	std::cout << "Block (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	this->indent_level++;
	for (auto& stmt : n->stmts)
	{
		stmt->accept(this);
	}
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(FuncDefnNode* n) 
{
	this->indent();
	std::cout << "FuncDefn (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	this->indent_level++;
	n->funcDecl->accept(this);
	n->funcBody->accept(this);
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(FuncDeclNode* n) 
{
	this->indent();
	std::cout << "FuncDecl (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	// start fund decl
	this->indent_level++;
	this->indent();
	std::cout << "Type: " << TypeNameString(n->t) << "\n"; 
	this->indent();
	std::cout << "Name: "<< n->name << "\n";
	this->indent();
	std::cout << "Params {\n";
	// start params
	this->indent_level++;
	for (auto& param : n->params)
	{
		param->accept(this);
	}
	// end params
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
	// end func decl
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(FuncCallNode* n) 
{
	this->indent();
	std::cout << "FuncCall (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n"; 
	this->indent_level++;
	this->indent();
	std:: cout << "Name: " << n->name << "\n";
	this->indent();
	std:: cout << "Args {\n";
	this->indent_level++;
	for (auto& arg : n->funcArgs)
	{
		arg->accept(this);
	}
	// end args
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
	// end func call
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(ConstantIntNode* n) 
{
	this->indent();
	std::cout << "Integer (" << n->location.begin.line << ", " << n->location.begin.column << ") { " << n->intValue << " }\n";
}

void PrintVisitor::visit(AssignmentNode* n) 
{
	this->indent();
	std::cout << "Assignment (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	this->indent_level++;
	this->indent();
	std::cout << "Name:	" << n->name << "\n";
	this->indent();
	std::cout << "=";
	n->expr->accept(this);
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(AugmentedAssignmentNode* n) 
{
	this->indent();
	std::cout << "AugmentedAssignment (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	this->indent_level++;
	this->indent();
	std::cout << "Name: " << n->name << "\n";
	this->indent();
	std::cout << AugmentedAssignOpsString(n->op) << "\n";
	n->expr->accept(this);
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(BoolNode* n) 
{
	this->indent();
	std::cout << "Bool (" << n->location.begin.line << ", " << n->location.begin.column << ") { " << (n->boolValue?"true":"false") << " }\n";
}

void PrintVisitor::visit(ReturnNode* n) 
{
	this->indent();
	std::cout << "Return (" << n->location.begin.line << ", " << n->location.begin.column << ") { ";
	if (n->expr)
	{
		std::cout << "\n";
		this->indent_level++;
		n->expr->accept(this);
		this->indent_level--;
		this->indent();
	}
	std::cout << "}\n";
}

void PrintVisitor::visit(ConstantFloatNode* n) 
{
	this->indent();
	std::cout << "Float (" << n->location.begin.line << ", " << n->location.begin.column << ") { " << n->floatValue << " }\n";
}

void PrintVisitor::visit(IfNode* n) 
{
	this->indent();
	std::cout << "If (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	this->indent_level++;
	n->ifExpr->accept(this);
	n->ifBody->accept(this);
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(ForNode* n) 
{
	this->indent();
	std::cout << "For (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	this->indent_level++;

	this->indent();
	std::cout << "InitStmt {\n";
	this->indent_level++;
	if (n->initStmt)
		n->initStmt->accept(this);
	else
	{
		this->indent();
		std::cout << "null\n";
	}
	this->indent_level--;
	this->indent();
	std::cout << "}\n";

	this->indent();
	std::cout << "LoopConditionExpr {\n";
	this->indent_level++;
	if (n->loopCondExpr)
		n->loopCondExpr->accept(this);
	else
	{
		this->indent();
		std::cout << "null\n";
	}
	this->indent_level--;
	this->indent();
	std::cout << "}\n";

	this->indent();
	std::cout << "UpdateStmt {\n";
	this->indent_level++;
	if (n->updateStmt)
		n->updateStmt->accept(this);
	else
	{
		this->indent();
		std::cout << "null\n";
	}
	this->indent_level--;
	this->indent();
	std::cout << "}\n";

	this->indent();
	std::cout << "LoopBody {\n";
	this->indent_level++;
	n->loopBody->accept(this);
	// end loop body
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
	
	// end for statement
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(WhileNode* n) 
{
	this->indent();
	std::cout << "While (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	this->indent_level++;
	n->whileExpr->accept(this);
	n->loopBody->accept(this);
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(UnaryNode* n) 
{
	this->indent();
	std::cout << "UnaryOp (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	this->indent_level++;
	this->indent();
	std::cout << "-\n";
	n->expr->accept(this);
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(TernaryNode* n) 
{
	this->indent();
	std::cout << "Ternary (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	this->indent_level++;
	n->condExpr->accept(this);
	this->indent();
	std::cout << "?\n";
	n->trueExpr->accept(this);
	this->indent();
	std::cout << ":\n";
	n->falseExpr->accept(this);
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(CastExpressionNode* n) 
{
	this->indent();
	std::cout << "Cast (" << n->location.begin.line << ", " << n->location.begin.column << ") {\n";
	this->indent_level++;
	this->indent();
	std::cout << "Type: " << TypeNameString(n->t) << "\n";
	n->expr->accept(this);
	this->indent_level--;
	this->indent();
	std::cout << "}\n";
}

void PrintVisitor::visit(ExpressionStatementNode* n)
{
	// This is a wrapper node, so such pass to child
	n->expr->accept(this);
}


void PrintVisitor::visit(BreakNode* n) 
{
	this->indent();
	std::cout << "Break (" << n->location.begin.line << ", " << n->location.begin.column << ")\n";
}

void PrintVisitor::visit(ContinueNode* n) 
{
	this->indent();
	std::cout << "Continue (" << n->location.begin.line << ", " << n->location.begin.column << ")\n";
}	

void PrintVisitor::indent()
{
	// indent two spaces per level
	std::cout << std::string(this->indent_level * 2, ' ');
}

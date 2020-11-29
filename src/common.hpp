/*
	Contains definitions used across multiple files.
	Tyler Weston for ECE467
*/

#ifndef ECE467_COMMON_HPP_INCLUDED
#define ECE467_COMMON_HPP_INCLUDED

#include <memory>
#include <string>
#include "llvm/IR/Type.h"
#include "llvm/IR/IRBuilder.h"

// templated helper functions for optimization of AST
template <class T> T _add(T a, T b) { return a + b; }
template <class T> T _sub(T a, T b) { return a - b; }
template <class T> T _mul(T a, T b) { return a * b; }
template <class T> T _div(T a, T b) { return a / b; }
// relational, return bool
template <class T> bool _eq(T a, T b) {	return a == b; }
template <class T> bool _ne(T a, T b) {	return a != b; }
template <class T> bool _lt(T a, T b) {	return a < b;  }
template <class T> bool _gt(T a, T b) {	return a > b;  }
template <class T> bool _le(T a, T b) {	return a <= b; }
template <class T> bool _ge(T a, T b) {	return a >= b; }
// Logical operators, return bool
template <class T> bool _land(T a, T b) { return a && b; }
template <class T> bool  _lor(T a, T b) { return a || b; }

// Forward declaration of Nodes for NodeVisitor
class VariableNode;
class DeclarationNode;
class DeclAndAssignNode;
class BinaryOpNode;
class LogicalOpNode;
class RelationalOpNode;
class RootNode;
class BlockNode;
class UnaryExpression;
class FuncDefnNode;
class FuncDeclNode;
class FuncCallNode;
class ConstantIntNode;
class AssignmentNode;
class AugmentedAssignmentNode;
class BoolNode;
class ReturnNode;
class ConstantFloatNode;
class IfNode;
class ForNode;
class WhileNode;
class UnaryNode;
class TernaryNode;
class CastExpressionNode;
class BreakNode;
class ContinueNode;
class ExpressionStatementNode;

class NodeVisitor
{
public:
	virtual void visit(VariableNode*) = 0;
	virtual void visit(DeclarationNode*) = 0;
	virtual void visit(DeclAndAssignNode*) = 0;
	virtual void visit(BinaryOpNode*) = 0;
	virtual void visit(LogicalOpNode*) = 0;
	virtual void visit(RelationalOpNode*) = 0;
	virtual void visit(RootNode*) = 0;
	virtual void visit(BlockNode*) = 0;
	virtual void visit(FuncDefnNode*) = 0;
	virtual void visit(FuncDeclNode*) = 0;
	virtual void visit(FuncCallNode*) = 0;
	virtual void visit(ConstantIntNode*) = 0;
	virtual void visit(AssignmentNode*) = 0;
	virtual void visit(AugmentedAssignmentNode*) = 0;
	virtual void visit(BoolNode*) = 0;
	virtual void visit(ReturnNode*) = 0;
	virtual void visit(ConstantFloatNode*) = 0;
	virtual void visit(IfNode*) = 0;
	virtual void visit(ForNode*) = 0;
	virtual void visit(WhileNode*) = 0;
	virtual void visit(UnaryNode*) = 0;
	virtual void visit(TernaryNode*) = 0;
	virtual void visit(CastExpressionNode*) = 0;
	virtual void visit(BreakNode*) = 0;
	virtual void visit(ContinueNode*) = 0;
	virtual void visit(ExpressionStatementNode*) = 0;
};

enum class TypeName
{
	tVoid,
	tInt,
	tFloat,
	tBool
};
std::string TypeNameString(TypeName t);
llvm::Type* GetLLVMType(TypeName t, llvm::IRBuilder<>* builder);

enum class BinaryOps
{
	Plus,
	Minus,
	Star,
	Slash,
	LogAnd,
	LogOr
};
std::string BinaryOpString(BinaryOps b);
llvm::Value* GetLLVMBinaryOp(BinaryOps b, llvm::IRBuilder<>* builder);

enum class RelationalOps
{
	Eq, 
	Ne, 
	Lt,
	Gt,
	Le, 
	Ge
};
std::string RelationalOpsString(RelationalOps r);

enum class AugmentedAssignOps
{
	PlusEq,
	MinusEq,
	StarEq,
	SlashEq
};
std::string AugmentedAssignOpsString(AugmentedAssignOps a);

#endif // ECE467_COMMON_HPP_INCLUDED

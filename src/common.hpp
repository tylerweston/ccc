/*
	Contains definitions used across multiple files.
	Tyler Weston for ECE467
*/

#ifndef CCC_COMMON_HPP_INCLUDED
#define CCC_COMMON_HPP_INCLUDED

#include <memory>
#include <string>


// templated helper functions for optimization of AST
template <class T> T _add(T a, T b) { return a + b; }
template <class T> T _sub(T a, T b) { return a - b; }
template <class T> T _mul(T a, T b) { return a * b; }
template <class T> T _div(T a, T b) { return a / b; }
template <class T> T _mod(T a, T b) { return a % b; }
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
// bitwise operators, return type T
template <class T> T _bw_and(T a, T b) { return a & b; }
template <class T> T _bw_or(T a, T b) { return a | b; }
template <class T> T _bw_xor(T a, T b) { return a ^ b; }
template <class T> T _bw_not(T a) { return ~a; }

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
class ConstantBoolNode;
class ConstantCharNode;
class ConstantDoubleNode;
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
	virtual void visit(ConstantBoolNode*) = 0;
	virtual void visit(ConstantCharNode*) = 0;
	virtual void visit(ConstantDoubleNode*) = 0;
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
	tBool,
	tChar,
	tDouble,
	tShort,
	tLong
};
std::string TypeNameString(TypeName t);

enum class BinaryOps
{
	Plus,
	Minus,
	Star,
	Slash,
	Mod,
	LogAnd,
	LogOr,
	BitAnd,
	BitOr,
	BitXor,
	LeftShift,
	RightShift
};
std::string BinaryOpString(BinaryOps b);

enum class UnaryOps
{
	Minus,
	Not
};
std::string UnaryOpString(UnaryOps u);

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

#endif // CCC_COMMON_HPP_INCLUDED


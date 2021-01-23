
#include "nodes.hpp"
#include "bridge.hpp"
#include "common.hpp"
#include "voptimize.hpp"
#include <memory>
#include <iostream>
#include <string>
#include <functional>

// Tyler Weston # 997438170 

/*
Optimization pass

- Simplifies binary, unary, and relational expressions whose operands are strictly constant
- if-statements with constant predicate (eliminate test, or entire statement)
- ternary operator with constant predicate (replace with the corresponding operand)
- while-statements with constant false predicate (eliminate the loop)

Whenever this visitor touches the tree it marks the tree as dirty and the optimizer will check the
tree again until it passes over the tree without optimizing anything.

There is LOTS of shared code here between these different functions,
but it is all different enough that it will take some clever abstraction
to make it work.

*/



// make node template from parser.y, for creating new nodes
template <typename T, typename... Args> static std::unique_ptr<T> make_node(YYLTYPE const& loc, Args&&... args) {
	std::unique_ptr<T> n = std::make_unique<T>(std::forward<Args>(args)...);
	n->location = loc;
	return n;
}

OptimizeVisitor::OptimizeVisitor()
{
	// init the optimize pass
	this->hasReplacement = false;
	this->removeNode = false;
	this->insertNodeVector = false;
}


void OptimizeVisitor::visit(VariableNode* n) 
{
	// nothing to optimize here
}

void OptimizeVisitor::visit(DeclarationNode* n) 
{
	// nothing to optimize here
}

void OptimizeVisitor::visit(DeclAndAssignNode* n) 
{
	// has an expr that we need to optimize
	n->expr->accept(this);
	if (this->hasReplacement)
	{
		// have to dynamic cast this to expression node?
		n->expr = std::move(this->repl_expr_node);
		this->cleanTree = false;
		this->hasReplacement = false;
	}
}

void OptimizeVisitor::visit(LogicalOpNode* n)
{
	/*
		TODO: This is basically the same code repeated three times
		there is probably a way to abstract this out but I'm not enough
		of a C++ whiz yet to know what it is.
	*/
	if (!(n->left->isConstant||n->right->isConstant))
		return;
	// has a left that we need to optimize
	n->left->accept(this);
	if (this->hasReplacement)
	{
		// have to dynamic cast this to expression node?
		n->left = std::move(this->repl_expr_node);
		this->cleanTree = false;
		this->hasReplacement = false;
	}
	// has a right that we need to optimize
	n->right->accept(this);
	if (this->hasReplacement)
	{
		// have to dynamic cast this to expression node?
		n->right = std::move(this->repl_expr_node);
		this->cleanTree = false;
		this->hasReplacement = false;
	}

	// now we may need to be optimized as well
	if (n->left->isConstant && n->right->isConstant)
	{
		// logical operator with int operands
		if (n->left->evaluatedType == TypeName::tInt)
		{
			int lvalue = dynamic_cast<ConstantIntNode*>(n->left.get())->intValue;
			int rvalue = dynamic_cast<ConstantIntNode*>(n->right.get())->intValue;

			//auto op = _add<int>;
			std::function<bool(int,int)> op;
			switch (n->op)
			{
				case BinaryOps::LogAnd:
					op = _land<int>;
					break;
				case BinaryOps::LogOr:
					op = _lor<int>;
					break;
				default:
					op = _land<int>;	// default placeholder, should never happen
					break;
			}

			bool result = op(lvalue, rvalue);
			this->repl_expr_node = make_node<ConstantBoolNode>(n->location, result);
			this->cleanTree = false;
			this->hasReplacement = true;
		}

		// logical operator with float operands
		if (n->right->evaluatedType == TypeName::tFloat)
		{
			float lvalue = dynamic_cast<ConstantFloatNode*>(n->left.get())->floatValue;
			float rvalue = dynamic_cast<ConstantFloatNode*>(n->right.get())->floatValue;

			std::function<bool(float,float)> op;
			switch (n->op)
			{
				case BinaryOps::LogAnd:
					op = _land<float>;
					break;
				case BinaryOps::LogOr:
					op = _lor<float>;
					break;
				default:
					op = _land<float>;	// default placeholder, should never happen
					break;
			}
			bool result = op(lvalue, rvalue);
			this->repl_expr_node = make_node<ConstantBoolNode>(n->location, result);
			this->cleanTree = false;
			this->hasReplacement = true;
		}

		// logical operator with bool operands
		if (n->right->evaluatedType == TypeName::tBool)
		{
			float lvalue = dynamic_cast<ConstantBoolNode*>(n->left.get())->floatValue;
			float rvalue = dynamic_cast<ConstantBoolNode*>(n->right.get())->floatValue;

			std::function<bool(bool,bool)> op;
			switch (n->op)
			{
				case BinaryOps::LogAnd:
					op = _land<bool>;
					break;
				case BinaryOps::LogOr:
					op = _lor<bool>;
					break;
				default:
					op = _land<bool>;	// default placeholder, should never happen
					break;
			}
			bool result = op(lvalue, rvalue);
			this->repl_expr_node = make_node<ConstantBoolNode>(n->location, result);
			this->cleanTree = false;
			this->hasReplacement = true;
		}
	}
}

void OptimizeVisitor::visit(BinaryOpNode* n) 
{
	/*
	TODO: This is basically the same code repeated three times
	there is probably a way to abstract this out but I'm not enough
	of a C++ whiz yet to know what it is.
	*/
	if (!(n->left->isConstant||n->right->isConstant))
		return;
	// has a left that we need to optimize
	n->left->accept(this);
	if (this->hasReplacement)
	{
		// have to dynamic cast this to expression node?
		n->left = std::move(this->repl_expr_node);
		this->cleanTree = false;
		this->hasReplacement = false;
	}
	// has a right that we need to optimize
	n->right->accept(this);
	if (this->hasReplacement)
	{
		// have to dynamic cast this to expression node?
		n->right = std::move(this->repl_expr_node);
		this->cleanTree = false;
		this->hasReplacement = false;
	}
	// now we may need to be optimized as well
	if (n->left->isConstant && n->right->isConstant)
	{
		// TODO: Handle LOG AND & LOG OR

		if (n->left->evaluatedType == TypeName::tInt)
		{
			int lvalue = dynamic_cast<ConstantIntNode*>(n->left.get())->intValue;
			int rvalue = dynamic_cast<ConstantIntNode*>(n->right.get())->intValue;

			std::function<int(int,int)> op;
			switch (n->op)
			{
				case BinaryOps::Plus:
					op = _add<int>;
					break;
				case BinaryOps::Minus:
					op = _sub<int>;
					break;
				case BinaryOps::Star:
					op = _mul<int>;
					break;
				case BinaryOps::Slash:
					op = _div<int>;
					break;
				default:
					op = _add<int>;	// default placeholder, should never happen
					break;
			}

			int result = op(lvalue, rvalue);
			this->repl_expr_node = make_node<ConstantIntNode>(n->location, result);
			this->cleanTree = false;
			this->hasReplacement = true;
		}
		if (n->right->evaluatedType == TypeName::tFloat)
		{
			float lvalue = dynamic_cast<ConstantFloatNode*>(n->left.get())->floatValue;
			float rvalue = dynamic_cast<ConstantFloatNode*>(n->right.get())->floatValue;

			std::function<float(float,float)> op;
			switch (n->op)
			{
				case BinaryOps::Plus:
					op = _add<float>;
					break;
				case BinaryOps::Minus:
					op = _sub<float>;
					break;
				case BinaryOps::Star:
					op = _mul<float>;
					break;
				case BinaryOps::Slash:
					op = _div<float>;
					break;
				default:
					op = _add<float>;	// default placeholder, should never happen
					break;
			}
			float result = op(lvalue, rvalue);
			this->repl_expr_node = make_node<ConstantFloatNode>(n->location, result);
			this->cleanTree = false;
			this->hasReplacement = true;
		}
		if (n->right->evaluatedType == TypeName::tDouble)
		{
			double lvalue = dynamic_cast<ConstantDoubleNode*>(n->left.get())->doubleValue;
			double rvalue = dynamic_cast<ConstantDoubleNode*>(n->right.get())->doubleValue;

			std::function<double(double,double)> op;
			switch (n->op)
			{
				case BinaryOps::Plus:
					op = _add<float>;
					break;
				case BinaryOps::Minus:
					op = _sub<float>;
					break;
				case BinaryOps::Star:
					op = _mul<float>;
					break;
				case BinaryOps::Slash:
					op = _div<float>;
					break;
				default:
					op = _add<float>;	// default placeholder, should never happen
					break;
			}
			double result = op(lvalue, rvalue);
			this->repl_expr_node = make_node<ConstantDoubleNode>(n->location, result);
			this->cleanTree = false;
			this->hasReplacement = true;
		}

	}
}

void OptimizeVisitor::visit(RelationalOpNode* n) 
{
	// if the ops are strictly constant, replace ths node with either
	// bool true or false

	n->left->accept(this);
	if (this->hasReplacement)
	{
		// have to dynamic cast this to expression node?
		n->left = std::move(this->repl_expr_node);
		this->cleanTree = false;
		this->hasReplacement = false;
	}
	// has a right that we need to optimize
	n->right->accept(this);
	if (this->hasReplacement)
	{
		// have to dynamic cast this to expression node?
		n->right = std::move(this->repl_expr_node);
		this->cleanTree = false;
		this->hasReplacement = false;
	}
	if (!(n->left->isConstant&&n->right->isConstant))
		return;
	// TODO: Gotta figure out a better way to do this!
	// is the same code a bunch of times! Maybe a template?
	// int relationals
	if (n->left->evaluatedType == TypeName::tInt)
	{
		int lvalue = dynamic_cast<ConstantIntNode*>(n->left.get())->intValue;
		int rvalue = dynamic_cast<ConstantIntNode*>(n->right.get())->intValue;

		std::function<bool(int,int)> op;
		switch (n->op)
		{
			case RelationalOps::Eq:
				op = _eq<int>;
				break;
			case RelationalOps::Ne:
				op = _ne<int>;
				break;
			case RelationalOps::Lt:
				op = _lt<int>;
				break;
			case RelationalOps::Gt:
				op = _gt<int>;
				break;
			case RelationalOps::Le:
				op = _le<int>;
				break;
			case RelationalOps::Ge:
				op = _ge<int>;
				break;
			default:
				op = _eq<int>;	// default placeholder, should never happen
				break;
		}

		bool result = op(lvalue, rvalue);
		this->repl_expr_node = make_node<ConstantBoolNode>(n->location, result);
		this->cleanTree = false;
		this->hasReplacement = true;
	}
	// relational float stuff	
	if (n->right->evaluatedType == TypeName::tFloat)
	{
		float lvalue = dynamic_cast<ConstantFloatNode*>(n->left.get())->floatValue;
		float rvalue = dynamic_cast<ConstantFloatNode*>(n->right.get())->floatValue;

		std::function<bool(float,float)> op;
		switch (n->op)
		{
			case RelationalOps::Eq:
				op = _eq<float>;
				break;
			case RelationalOps::Ne:
				op = _ne<float>;
				break;
			case RelationalOps::Lt:
				op = _lt<float>;
				break;
			case RelationalOps::Gt:
				op = _gt<float>;
				break;
			case RelationalOps::Le:
				op = _le<float>;
				break;
			case RelationalOps::Ge:
				op = _ge<float>;
				break;
			default:
				op = _eq<float>;	// default placeholder, should never happen
				break;
		}
		bool result = op(lvalue, rvalue);
		this->repl_expr_node = make_node<ConstantBoolNode>(n->location, result);
		this->cleanTree = false;
		this->hasReplacement = true;
	}

	// relational double stuff	
	if (n->right->evaluatedType == TypeName::tDouble)
	{
		double lvalue = dynamic_cast<ConstantDoubleNode*>(n->left.get())->doubleValue;
		double rvalue = dynamic_cast<ConstantDoubleNode*>(n->right.get())->doubleValue;

		std::function<bool(double,double)> op;
		switch (n->op)
		{
			case RelationalOps::Eq:
				op = _eq<double>;
				break;
			case RelationalOps::Ne:
				op = _ne<double>;
				break;
			case RelationalOps::Lt:
				op = _lt<double>;
				break;
			case RelationalOps::Gt:
				op = _gt<double>;
				break;
			case RelationalOps::Le:
				op = _le<double>;
				break;
			case RelationalOps::Ge:
				op = _ge<double>;
				break;
			default:
				op = _eq<double>;	// default placeholder, should never happen
				break;
		}
		bool result = op(lvalue, rvalue);
		this->repl_expr_node = make_node<ConstantBoolNode>(n->location, result);
		this->cleanTree = false;
		this->hasReplacement = true;
	}

	// relational char stuff	
	if (n->right->evaluatedType == TypeName::tChar)
	{
		char lvalue = dynamic_cast<ConstantCharNode*>(n->left.get())->charValue;
		char rvalue = dynamic_cast<ConstantCharNode*>(n->right.get())->charValue;

		std::function<bool(char,char)> op;
		switch (n->op)
		{
			case RelationalOps::Eq:
				op = _eq<char>;
				break;
			case RelationalOps::Ne:
				op = _ne<char>;
				break;
			case RelationalOps::Lt:
				op = _lt<char>;
				break;
			case RelationalOps::Gt:
				op = _gt<char>;
				break;
			case RelationalOps::Le:
				op = _le<char>;
				break;
			case RelationalOps::Ge:
				op = _ge<char>;
				break;
			default:
				op = _eq<char>;	// default placeholder, should never happen
				break;
		}
		bool result = op(lvalue, rvalue);
		this->repl_expr_node = make_node<ConstantBoolNode>(n->location, result);
		this->cleanTree = false;
		this->hasReplacement = true;
	}



	// Support optimizing == and != for bool type	
	if (n->right->evaluatedType == TypeName::tBool && (n->op == RelationalOps::Eq || n->op == RelationalOps::Ne))
	{
		float lvalue = dynamic_cast<ConstantBoolNode*>(n->left.get())->boolValue;
		float rvalue = dynamic_cast<ConstantBoolNode*>(n->right.get())->boolValue;

		std::function<bool(bool,bool)> op;
		switch (n->op)
		{
			case RelationalOps::Eq:
				op = _eq<bool>;
				break;
			case RelationalOps::Ne:
				op = _ne<bool>;
				break;
			default:
				op = _eq<bool>;	// default placeholder, should never happen
				break;
		}
		bool result = op(lvalue, rvalue);
		this->repl_expr_node = make_node<ConstantBoolNode>(n->location, result);
		this->cleanTree = false;
		this->hasReplacement = true;
	}
}

void OptimizeVisitor::visit(RootNode* n) 
{
	// nothing in RootNode to optimzie, so just visit our functions
	for (auto& func : n->funcs)
	{
		func->accept(this);
	}	
}

void OptimizeVisitor::visit(BlockNode* n) 
{
	// this is the body of functions so this is where if/while loops, etc. will appear
	//bool markedForRemove[n->stmts.size()] = {0};
	std::vector<bool> markedForRemove(n->stmts.size(), false);
	unsigned insertion_index = 0;
	for (unsigned i = 0; i < n->stmts.size(); i++)
	{
		n->stmts[i]->accept(this);
		if (this->hasReplacement)
		{
			n->stmts[i] = std::move(this->replacement_node);
			this->cleanTree = false;
			this->hasReplacement = false;
		}

		if (this->removeNode)
		{
			markedForRemove[i] = true;
			this->cleanTree = false;
			this->removeNode = false;
			break;	// only deal with one insertion/removal at a time
		}

		if (this->insertNodeVector)
		{
			// if we need to bounce down our if loop, we do it here
			n->stmts.insert(n->stmts.begin() + insertion_index + 1, 
				std::make_move_iterator(this->node_list.begin()), 
				std::make_move_iterator(this->node_list.end()));	// can insert an entire vector?

			// todo: hmmmm how to make this work


			markedForRemove[i] = true;
			this->cleanTree = false;
			this->insertNodeVector = false;
			break;
		}
		insertion_index++;
	}
	// remove any nodes marked for removal, we do this in a different
	// loop so we don't mess up the order of our commands or anything 
	// above!
	unsigned i = 0;
	for(auto stmtit = n->stmts.begin(); stmtit!=n->stmts.end();)
	{
		if (markedForRemove[i])
		{
			stmtit = n->stmts.erase(stmtit);
		}
		else
		{
			++stmtit;
		}
		++i;
	}
}

void OptimizeVisitor::visit(FuncDefnNode* n) 
{
	// visit the body to optimize it
	n->funcBody->accept(this);
}

void OptimizeVisitor::visit(FuncDeclNode* n) 
{
	// Nothing to optimize
}

void OptimizeVisitor::visit(FuncCallNode* n) 
{
	// func args may be optimized
	for (unsigned i = 0; i < n->funcArgs.size(); i++)
	{
		n->funcArgs[i]->accept(this);
		if (this->hasReplacement)
		{
			n->funcArgs[i] = std::move(this->repl_expr_node);
			this->cleanTree = false;
			this->hasReplacement = false;
		}
	}
}

void OptimizeVisitor::visit(ConstantIntNode* n) 
{
	// nothing to optimize
}

void OptimizeVisitor::visit(AssignmentNode* n) 
{
	// has an expr that we need to optimize
	n->expr->accept(this);
	if (this->hasReplacement)
	{
		// have to dynamic cast this to expression node?
		n->expr = std::move(this->repl_expr_node);
		this->cleanTree = false;
		this->hasReplacement = false;
	}
}

void OptimizeVisitor::visit(AugmentedAssignmentNode* n) 
{
	// has as expr that needs to be optimized
	// has an expr that we need to optimize
	n->expr->accept(this);
	if (this->hasReplacement)
	{
		// have to dynamic cast this to expression node?
		n->expr = std::move(this->repl_expr_node);
		this->cleanTree = false;
		this->hasReplacement = false;
	}
}

void OptimizeVisitor::visit(ConstantBoolNode* n) 
{
	// nothing to optimize
}

void OptimizeVisitor::visit(ReturnNode* n) 
{
	// MAY have an expr (or nullptr) that needs to be optimized
	if (n->expr)
	{
		// has an expr that we need to optimize
		n->expr->accept(this);
		if (this->hasReplacement)
		{
			// have to dynamic cast this to expression node?
			n->expr = std::move(this->repl_expr_node);
			this->cleanTree = false;
			this->hasReplacement = false;
		}
	}
}

void OptimizeVisitor::visit(ConstantFloatNode* n) 
{
	// nothing to optimize
}

void OptimizeVisitor::visit(ConstantCharNode* n)
{
	// nothing to do
}

void OptimizeVisitor::visit(ConstantDoubleNode* n)
{
	// nothing to do
}

void OptimizeVisitor::visit(IfNode* n) 
{
	// if the predicate is always false, remove the entire if loop
	// if the predicate is always true, pop the entire body out into the outer loop
	// TODO: Don't worry about variable shadowing
	n->ifExpr->accept(this);
	if (this->hasReplacement)
	{
		// have to dynamic cast this to expression node?
		n->ifExpr = std::move(this->repl_expr_node);
		this->cleanTree = false;
		this->hasReplacement = false;
	}
	// if we have a constant node, simplify that
	if (n->ifExpr->isConstant && n->ifExpr->evaluatedType == TypeName::tBool)
	{
		// grab the bool value from the simplified node 
		bool nvalue = dynamic_cast<ConstantBoolNode*>(n->ifExpr.get())->boolValue;
		// // replace this node with the appropriate operand
		// this->repl_expr_node = std::move(nvalue ? n->trueExpr : n->falseExpr);
		// this->cleanTree = false;
		// this->hasReplacement = true;
		if (!nvalue)
		{
			this->removeNode = true;		
		}
		else
		{
			// here, we have to add all of our
			// blocks elements to 
			this->insertNodeVector = true;
			this->node_list = std::move(dynamic_cast<BlockNode*>(n->ifBody.get())->stmts);
		}
	}	
}

void OptimizeVisitor::visit(ForNode* n) 
{
	// may have a midExpr that needs to be optimized
	if (n->loopCondExpr)
	{
		n->loopCondExpr->accept(this);
		if (this->hasReplacement)
		{
			// have to dynamic cast this to expression node?
			n->loopCondExpr = std::move(this->repl_expr_node);
			this->cleanTree = false;
			this->hasReplacement = false;
		}
	}
	// then visit the body
	n->loopBody->accept(this);
}

void OptimizeVisitor::visit(WhileNode* n) 
{
	// if the predicate is always false, remove this
	n->whileExpr->accept(this);
	if (this->hasReplacement)
	{
		// have to dynamic cast this to expression node?
		n->whileExpr = std::move(this->repl_expr_node);
		this->cleanTree = false;
		this->hasReplacement = false;
	}
	// if we have a constant node that is a bool, and the value of that
	// bool is false, mark this node for removal!
	// NOTE: this HAS to be a bool because of error checking we did earlier?
	if (n->whileExpr->isConstant && n->whileExpr->evaluatedType == TypeName::tBool)
	{
		bool nvalue = dynamic_cast<ConstantBoolNode*>(n->whileExpr.get())->boolValue;
		if (!nvalue)
		{
			this->removeNode = true;
		}
	}
	// visit the body of the while loop
	n->loopBody->accept(this);
}

void OptimizeVisitor::visit(UnaryNode* n) 
{
	// this will have one expression, if it is marked constant
	// we can replace it with -expression
	// IF we had more unary oprators, say ~ or !, we'd have
	// to keep track of the operator and apply the proper 
	// transformation below

	// evaluate expr and replace it with simplified ver
	n->expr->accept(this);
	if (this->hasReplacement)
	{
		// have to dynamic cast this to expression node?
		n->expr = std::move(this->repl_expr_node);
		this->cleanTree = false;
		this->hasReplacement = false;
	}
	// if we have a constant node, simplify that
	if (n->expr->isConstant)
	{
		if (n->expr->evaluatedType == TypeName::tInt)
		{
			// we're an int node! So make a new negative int node
			int value = dynamic_cast<ConstantIntNode*>(n->expr.get())->intValue;
			this->repl_expr_node = make_node<ConstantIntNode>(n->location, -value);
			this->cleanTree = false;
			this->hasReplacement = true;	
		}
		if (n->expr->evaluatedType == TypeName::tFloat)
		{
			// we're a float node, so dittio
			float value = dynamic_cast<ConstantFloatNode*>(n->expr.get())->floatValue;
			this->repl_expr_node = make_node<ConstantFloatNode>(n->location, -value);
			this->cleanTree = false;
			this->hasReplacement = true;			
		}
	}
}

void OptimizeVisitor::visit(TernaryNode* n) 
{
	// if the condition is always true or false, replace it with the appropriate
	// operand
	// evaluate expr and replace it with simplified ver
	n->condExpr->accept(this);
	if (this->hasReplacement)
	{
		// have to dynamic cast this to expression node?
		n->condExpr = std::move(this->repl_expr_node);
		this->cleanTree = false;
		this->hasReplacement = false;
	}
	// if we have a constant node, simplify that
	if (n->condExpr->isConstant && n->condExpr->evaluatedType == TypeName::tBool)
	{
		// grab the bool value from the simplified node 
		bool nvalue = dynamic_cast<ConstantBoolNode*>(n->condExpr.get())->boolValue;
		// replace this node with the appropriate operand
		this->repl_expr_node = std::move(nvalue ? n->trueExpr : n->falseExpr);
		this->cleanTree = false;
		this->hasReplacement = true;
	}	
}

void OptimizeVisitor::visit(CastExpressionNode* n) 
{
	// has an expr that we need to optimize
	n->expr->accept(this);
	if (this->hasReplacement)
	{
		n->expr = std::move(this->repl_expr_node);
		this->cleanTree = false;
		this->hasReplacement = false;
	}
	// if we are trying to cast to the same type our operand already is, optimize out
	if (n->t == n->expr->evaluatedType)
	{
		this->repl_expr_node = std::move(n->expr);
		this->cleanTree = false;
		this->hasReplacement = true;
	}
}

void OptimizeVisitor::visit(BreakNode* n) 
{
	// nothing to optimize here
	// maybe some other node will have something
	// for you to optimize!
}

void OptimizeVisitor::visit(ContinueNode* n) 
{
	// nothing to optimize here
	// move along! nothing to see here!
}	

void OptimizeVisitor::visit(ExpressionStatementNode* n) 
{
	// has an expression that could be optimized
	n->expr->accept(this);
	if (this->hasReplacement)
	{
		n->expr = std::move(this->repl_expr_node);
		this->cleanTree = false;
		this->hasReplacement = false;
	}
}

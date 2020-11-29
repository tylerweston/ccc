/*
	common.cpp
*/
#include "common.hpp"

std::string TypeNameString(TypeName t)
{
	switch(t) 
	{
		case TypeName::tVoid:
			return "void";
		case TypeName::tInt:
			return "int";
		case TypeName::tFloat:
			return "float";
		case TypeName::tBool:
			return "bool";
		default:
			return "undefined";
	}
}



std::string BinaryOpString(BinaryOps b)
{
	switch(b) 
	{
		case BinaryOps::Plus:
			return "+";
		case BinaryOps::Minus:
			return "-";
		case BinaryOps::Star:
			return "*";
		case BinaryOps::Slash:
			return "/";
		case BinaryOps::LogAnd:
			return "&&";
		case BinaryOps::LogOr:
			return "||";
		default:
			return "undefined";
	}
}



std::string RelationalOpsString(RelationalOps r)
{
	switch(r) 
	{
		case RelationalOps::Eq:
			return "==";
		case RelationalOps::Ne:
			return "!=";
		case RelationalOps::Lt:
			return "<";
		case RelationalOps::Gt:
			return ">";
		case RelationalOps::Le:
			return "<=";
		case RelationalOps::Ge:
			return ">=";
		default:
			return "undefined";
	}
}

std::string AugmentedAssignOpsString(AugmentedAssignOps a)
{
	switch(a) 
	{
		case AugmentedAssignOps::PlusEq:
			return "+=";
		case AugmentedAssignOps::MinusEq:
			return "-=";
		case AugmentedAssignOps::StarEq:
			return "*=";
		case AugmentedAssignOps::SlashEq:
			return "/=";
		default:
			return "undefined";
	}
}

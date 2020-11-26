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
		case TypeName::tUnknown:
			return "Unknown";
	}
	return "Unknown";
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
	}
	return "Unknown";
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
	}
	return "Unknown";
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
	}
	return "Unknown";
}

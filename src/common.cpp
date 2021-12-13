/*
	common.cpp
*/
#include "common.hpp"
#include <map>
#include <string>

std::string TypeNameString(TypeName t)
{
	std::map<TypeName, std::string> TypeNameStringTable = {
		{TypeName::tVoid, 	"void"},
		{TypeName::tBool, 	"bool"},
		{TypeName::tChar, 	"char"},
		{TypeName::tInt, 	"int"},
		{TypeName::tFloat, 	"float"},
		{TypeName::tDouble, "double"},
		{TypeName::tShort, 	"short"},
		{TypeName::tLong, 	"long"},
	};
	if (TypeNameStringTable.find(t) == TypeNameStringTable.end())
		return "undefined type";
	return TypeNameStringTable[t];
}



std::string BinaryOpString(BinaryOps b)
{
	std::map<BinaryOps, std::string> BinaryOpStringTable = {
		{BinaryOps::Plus, 		"+"},
		{BinaryOps::Minus, 		"-"},
		{BinaryOps::Star, 		"*"},
		{BinaryOps::Slash, 		"/"},
		// {BinaryOps::Mod, 		"%"},
		{BinaryOps::LogAnd, 	"&&"},
		{BinaryOps::LogOr, 		"||"},
	};
	if (BinaryOpStringTable.find(b) == BinaryOpStringTable.end())
		return "undefined binary operator";
	return BinaryOpStringTable[b];
}



std::string RelationalOpsString(RelationalOps r)
{
	std::map<RelationalOps, std::string> RelationalOpsStringTable = {
		{RelationalOps::Eq, 		"=="},
		{RelationalOps::Ne, 		"!="},
		{RelationalOps::Lt, 		"<"},
		{RelationalOps::Le, 		"<="},
		{RelationalOps::Gt, 		">"},
		{RelationalOps::Ge, 		">="},
	};
	if (RelationalOpsStringTable.find(r) == RelationalOpsStringTable.end())
		return "undefined relational operator";
	return RelationalOpsStringTable[r];
}

std::string AugmentedAssignOpsString(AugmentedAssignOps a)
{
	std::map<AugmentedAssignOps, std::string> AugmentedAssignOpsStringTable = {
		{AugmentedAssignOps::PlusEq, 		"+="},
		{AugmentedAssignOps::MinusEq, 		"-="},
		{AugmentedAssignOps::StarEq, 		"*="},
		{AugmentedAssignOps::SlashEq, 		"/="},

	};
	if (AugmentedAssignOpsStringTable.find(a) == AugmentedAssignOpsStringTable.end())
		return "undefined augmented assign operator";
	return AugmentedAssignOpsStringTable[a];
}

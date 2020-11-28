/*
	common.cpp
*/
#include "common.hpp"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/IRBuilder.h"

// llvm::Type* GetLLVMType(TypeName t, llvm::LLVMContext& context)
// {
// 	switch(t) 
// 	{
// 		case TypeName::tVoid:
// 			return llvm::Type::getVoidTy(context);
// 		case TypeName::tInt:
// 			return llvm::Type::getIntNTy(context, 32);
// 		case TypeName::tFloat:
// 			return llvm::Type::getFloatTy(context);
// 		case TypeName::tBool:
// 			return llvm::Type::getInt1TY(context);
// 	}
// }

llvm::Type* GetLLVMType(TypeName t, llvm::IRBuilder<>* builder)
{
	switch(t) 
	{
		case TypeName::tVoid:
			return builder->getVoidTy();
		case TypeName::tInt:
			return builder->getInt32Ty();
		case TypeName::tFloat:
			return builder->getFloatTy();
		case TypeName::tBool:
			return builder->getInt1Ty();
		default:
			// TODO: llvm unreachable
			return builder->getInt32Ty();
	}
}


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
			// TODO: llvm unreachable
			return "int";
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

/*
 symtable.hpp
 header file for managing symbol table
*/

#ifndef ECE467_SYMTABLE_HPP_INCLUDED
#define ECE467_SYMTABLE_HPP_INCLUDED

#include <string>
#include <map>
#include <vector>
#include <list>		// used as stack, maybe change to vector?
#include "common.hpp"
#include "bridge.hpp"
#include "llvm/IR/ValueHandle.h"
#include "llvm/IR/Instructions.h"

typedef struct 
{
	std::string Name;				// name of symbol
	TypeName Type;					// type of symbol
	YYLTYPE declarationLocation;	// location where symbol was defined
	// Additional attributes would go here, CONST, etc.
	llvm::AllocaInst* val;				// use this to hold llvm specific info, usually a pointer to a memory location
} SymbolTableEntry;

typedef struct 
{
	std::string Name;				// name of function
	TypeName ReturnType;			// return type of function
	std::vector<TypeName> ParamTypes;	// types of parameters of function
	bool hasDefinition;				// we can declare a function then define it later. It is not an error
	YYLTYPE definitionLocation;		// where did we see the function defined
} FunctionTableEntry;

// Make our life easy with some typedefs
// A symbol table is a map of string to SymbolTableEntry*
typedef std::map<std::string, SymbolTableEntry*> SymbolTableType;
// A function table is a map of string to FunctionTableEntry*
typedef std::map<std::string, FunctionTableEntry*> FunctionTableType;

class SymbolTable
{
private:
	std::list<SymbolTableType*> SymbolTableStack;	// our list of symbol tables
	SymbolTableType* CurrentSymbolTable;			// To keep track of current symbol table
	
public:
	SymbolTable();
	SymbolTableEntry* GetSymbol(std::string Symbol);
	llvm::AllocaInst* GetLLVMValue(std::string Symbol);
	bool AddSymbol(std::string Name, TypeName Type, YYLTYPE loc);
	bool AddLLVMSymbol(std::string Name, llvm::AllocaInst* val);

	void PushScope();
	void PopScope();

	void PrintSymbolTable();
	void CleanUpSymbolTable();
};

// Function table
class FunctionTable
{
private:
	FunctionTableType* funcTable;
	FunctionTableEntry* currentFunction;

public:
	FunctionTable();
	FunctionTableEntry* GetCurrentFunction();
	FunctionTableEntry* GetFunction(std::string Name);
	bool AddFunction(std::string Name, TypeName ReturnType, std::vector<TypeName> ParamTypes);
	void DefineFunction(std::string Name, YYLTYPE loc);
	bool IsInFunctionDefinition();
	bool IsFunctionDefined(std::string Name);

	void EnterFunctionDefinition(std::string Name);
	void ExitFunctionDefinition();

	void PrintFunctionTable();
	void CleanUpFunctionTable();
};

#endif	// ECE467_SYMTABLE_HPP_INCLUDED

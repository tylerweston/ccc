/*
   symtable.cpp
   implementations for symbol table
   Tyler Weston ece467
*/

#include "symtable.hpp"
#include <vector>
#include <list>

SymbolTable::SymbolTable()
{
	// init our list and tables here
	SymbolTableStack = std::list<SymbolTableType*>();
	PushScope();	// start our main scope
}

bool SymbolTable::AddSymbol(std::string name, TypeName type, YYLTYPE loc)
{
	// add this symbol to the currently in scope symbol table
	// return true if it succeeded or false if this symbol already 
	// exists in this scope
	if (CurrentSymbolTable->find(name) != CurrentSymbolTable->end())
	{
		return false;	// already declared in this scope
	}
	// make our new symbol table entry
	SymbolTableEntry* symbolTableEntry = new SymbolTableEntry();
	symbolTableEntry->Name = name;
	symbolTableEntry->Type = type;
	symbolTableEntry->declarationLocation = loc;
	// add it to the current active symbol table
	CurrentSymbolTable->insert(std::pair<std::string, SymbolTableEntry*>(name, symbolTableEntry));
	return true;
}

bool SymbolTable::AddLLVMSymbol(std::string name, llvm::AllocaInst* val)
{
	// add this symbol to the currently in scope symbol table
	// return true if it succeeded or false if this symbol already 
	// exists in this scope. This is for use in the llvm IR generation
	// phase so the only thing we need is the llvm::Value*
	if (CurrentSymbolTable->find(name) != CurrentSymbolTable->end())
	{
		return false;	// already declared in this scope
	}
	// make our new symbol table entry
	SymbolTableEntry* symbolTableEntry = new SymbolTableEntry();
	symbolTableEntry->Name = name;
	symbolTableEntry->val = val;
	// add it to the current active symbol table
	CurrentSymbolTable->insert(std::pair<std::string, SymbolTableEntry*>(name, symbolTableEntry));
	return true;
}

void SymbolTable::PushScope()
{								
	// enter a new scope
	SymbolTableType* NewSymbolTable = new SymbolTableType();
	this->SymbolTableStack.push_back(NewSymbolTable);
	CurrentSymbolTable = this->SymbolTableStack.back();
}

void SymbolTable::PopScope()
{
	// exit from our current scope
	if (this->SymbolTableStack.size() == 0)
	{
		// we're attempting to pop on an empty symbol stack, something's gone wrong!
		// TODO: show an error message or throw an error or something?? Handle this somewhere?
		return;
	}
	SymbolTableType* del = this->SymbolTableStack.back();
	this->SymbolTableStack.pop_back();							// Remove and clean up symbol table when we're
	for (auto& st: *del)
	{
		delete(st.second);
	}
	delete(del);												// finished with it
	CurrentSymbolTable = this->SymbolTableStack.back();
}

SymbolTableEntry* SymbolTable::GetSymbol(std::string Symbol)
{
	// return the Symbol from the topmost scoped symbol table
	// if none, return NULL or NULLPTR or 0 or something?
	// TODO: Test this? Not sure if this even really works?
	// since it wasn'a a priority for this Lab
	for (auto iter = SymbolTableStack.rbegin();
		iter != SymbolTableStack.rend();
		iter++)
	{
		auto found = (*iter)->find(Symbol);	
		if (found == (*iter)->end())
		{
			continue;
		}
		return found->second;
	}
	return nullptr;
}

llvm::AllocaInst* SymbolTable::GetLLVMValue(std::string Symbol)
{
	SymbolTableEntry* s = this->GetSymbol(Symbol);
	if (s)
		return s->val;
	return nullptr;
}

void SymbolTable::PrintSymbolTable()
{
	// For debugging purposes
	// TODO
}

void SymbolTable::CleanUpSymbolTable()
{
	// free memory up
	for (auto& s : SymbolTableStack)
	{
		for (auto& st: *s)
		{
			delete(st.second);
		}
		delete(s);
	}
}

// Function table stuff goes here
FunctionTable::FunctionTable()
{
	// Initialize our function table
	funcTable = new FunctionTableType();
	this->currentFunction = nullptr;

}

bool FunctionTable::AddFunction(std::string Name, TypeName ReturnType, std::vector<TypeName> ParamTypes)
{
	// Add a function table entry
	FunctionTableEntry* funcTableEntry = new FunctionTableEntry();
	funcTableEntry->Name = Name;
	funcTableEntry->ReturnType = ReturnType;
	funcTableEntry->ParamTypes = std::move(ParamTypes);
	funcTableEntry->hasDefinition = false;
	funcTable->insert(std::pair<std::string, FunctionTableEntry*>(Name, funcTableEntry));
	return true;
}
void FunctionTable::PrintFunctionTable()
{
	// Display Function Table entries for debugging purposes
	// TODO: For now, debug with GDB like a pro!
}

bool FunctionTable::IsFunctionDefined(std::string Name)
{
	// Check the function table to see if we have a definition for
	// a function or just a declaration
	FunctionTableEntry* ffunc = this->GetFunction(Name);
	if (!ffunc)
	{
		return false;
	}
	return ffunc->hasDefinition;
}

void FunctionTable::EnterFunctionDefinition(std::string Name)
{
	// Helper for evaluator to keep track of function declaration / definition
	this->currentFunction = this->GetFunction(Name);
}

void FunctionTable::ExitFunctionDefinition()
{
	// Helper for evaluator to keep track of function declaration / definition
	this->currentFunction = nullptr;
}

void FunctionTable::DefineFunction(std::string Name, YYLTYPE loc)
{
	// Once we get a function definition (as opposed to a declaration),
	// we store some extra info in our function table
	FunctionTableEntry* ffunc = this->GetFunction(Name);
	if (!ffunc)
	{
		return;
	}
	ffunc->definitionLocation = loc;
	ffunc->hasDefinition = true;
}

bool FunctionTable::IsInFunctionDefinition()
{
	// A kind of hacky way to test if we are currently parsing a
	// function declaration or definition
	return this->currentFunction != nullptr;
}

FunctionTableEntry* FunctionTable::GetCurrentFunction()
{
	// Return the function we are currently in while evaluating the AST
	return this->currentFunction;
}

FunctionTableEntry* FunctionTable::GetFunction(std::string Name)
{
	// Return the function table entry associated with name
	auto found = funcTable->find(Name);
	if (found == funcTable->end())
	{
		return nullptr;	// already declared in this scope
	}
	return found->second;	
}

void FunctionTable::CleanUpFunctionTable()
{
	// Clean up function table remenants
	for (auto& it:*funcTable)
	{
		delete(it.second);
	}
	delete(funcTable);
}

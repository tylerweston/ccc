// Given
#include "headers/compiler.hpp"
#include "headers/lexer.hpp"
#include "parser.hpp"	// This doesn't live in headers/ because it is auto generated

// Common
#include "headers/nodes.hpp"
#include "headers/common.hpp"

// Visitors
#include "headers/vprint.hpp"
#include "headers/voptimize.hpp"
#include "headers/vevaluate.hpp"
#include "headers/vcodegen.hpp"

// Symbol & Function table
#include "headers/symtable.hpp"

// LLVM
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Bitcode/BitcodeWriter.h"
// unused
// #include "llvm/ExecutionEngine/Orc/LLJIT.h"
// #include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"

// Standard Libraries.
// TODO: Change printfs to std::couts
#include <cstdio>
#include <cassert>
#include <cstring>

int lex(const std::string& path) 
{
	FILE* in = fopen(path.c_str(), "r");
	if (in == nullptr) {
		printf("[error] unable to open file: %s\n", std::strerror(errno));
		return 1;
	}

	yyscan_t lexer;
	yylex_init_extra(1, &lexer);

	yyset_in(in, lexer);

	while (true) {
		yy::parser::symbol_type s;
		int x = yylex(&s, nullptr, lexer);
		assert(x == 1);
		printf("[output] lexer got symbol: %s (%d, %d).\n", s.name(), s.location.begin.line, s.location.begin.column);
		// TODO: figure out cleaner way to print lexer output
		// printf ("%s ", s.name());
		// if (s.kind() == yy::parser::symbol_kind_type::S_YYEMPTY)
		// {
		// 	printf("\n");
		// }
		if (s.kind() == yy::parser::symbol_kind_type::S_YYEOF) 
		{
			break;
		}
	}

	yylex_destroy(lexer);
	fclose(in);

	return 0;
}

int parse(const std::string& path, std::unique_ptr<Node>& root) 
{
	FILE* in = fopen(path.c_str(), "r");
	if (in == nullptr) {
		printf("[error] unable to open file: %s\n", std::strerror(errno));
		return 1;
	}

	yyscan_t lexer;
	yylex_init_extra(1, &lexer);

	yyset_in(in, lexer);

	yy::parser p(lexer, root);
	int x = p.parse();

	yylex_destroy(lexer);
	fclose(in);

	return x;
}

bool verify_ast(Node* root) 
{
	// Semantic check of our generated AST

	// create our symbol table and function table
	// if we need this for code gen or anything it can be pushed up a level
	// and just passed in as a parameter here or a something.
	SymbolTable* symbolTable = new SymbolTable();
	FunctionTable* functionTable = new FunctionTable();

	// Create our semantic analyzer visitor
	EvaluateVisitor evaluateVisitor;
	evaluateVisitor.symbolTable = symbolTable;
	evaluateVisitor.functionTable = functionTable;
	root->accept(&evaluateVisitor);

	// Make sure we got a main function and that it returns an int
	FunctionTableEntry* mainf = evaluateVisitor.functionTable->GetFunction("main");
	if (mainf == nullptr)
	{
		printf("Error: No main function found\n");
		exit(1);
	}
	if (mainf->ReturnType != TypeName::tInt)	// this is already checked in evaluate?
	{
		printf("Error: Main function needs return type int\n");
		exit(1);
	}

	// Clean up our symbol table and function table
	symbolTable->CleanUpSymbolTable();
	functionTable->CleanUpFunctionTable();
	delete(symbolTable);
	delete(functionTable);

	return true;	// if we get here, we haven't hit any semantic errors
}

std::unique_ptr<Node> optimize(std::unique_ptr<Node> root) 
{
	// Optimize our AST by performing some simplifications
	OptimizeVisitor optimizeVisitor;

	do
	{
		optimizeVisitor.cleanTree = true;
		root->accept(&optimizeVisitor);
	}
	while (!optimizeVisitor.cleanTree);	// repeat until we iterate through the tree
										// without making any changes to it
	return root;
}

void print_ast(Node* root) 
{
	// Use Visitor pattern to print our generated AST
	PrintVisitor printVisitor;
	root->accept(&printVisitor);
	return;
}

std::unique_ptr<CompilationUnit> compile(Node* root) 
{
	// run the  compilation process
	std::unique_ptr<CompilationUnit> unit = std::make_unique<CompilationUnit>();
	if (!unit->process(root)) {
		return nullptr;
	}
	return unit;
}

void CompilationUnit::initialize() 
{
	llvm::InitializeNativeTarget();
}

CompilationUnit::CompilationUnit() : context(std::make_unique<llvm::LLVMContext>()), builder(*this->context) 
{
	// todo: make this module named after the filename
	this->module = std::make_unique<llvm::Module>("ccc", *this->context);
}

bool CompilationUnit::process(Node* root) 
{
	// Generate our llvm IR code
	CodegenVisitor codegenVisitor;
	codegenVisitor.compilationUnit = this;
	root->accept(&codegenVisitor);

	llvm::verifyModule(*this->module, &llvm::errs());
	return true;
}

std::error_code CompilationUnit::dump(std::string path, int print_ir) 
{
	std::error_code ec;
	llvm::raw_fd_ostream out(path, ec, llvm::sys::fs::OpenFlags::OF_None);
	this->module->print(out, nullptr);
	if (print_ir)
	{
		// print our generated llvm to stdout
		this->module->print(llvm::outs(), nullptr);
	}
	return ec;
}

#ifndef CCC_COMPILER_HPP_INCLUDED
#define CCC_COMPILER_HPP_INCLUDED

#include "llvm/Support/Error.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include <string>
#include <memory>
#include <optional>
#include <functional>
#include <system_error>

class Node;
class CompilationUnit;

int lex(char const*);
int parse(char const*, std::unique_ptr<Node>&);
bool verify_ast(Node*);
std::unique_ptr<Node> optimize(std::unique_ptr<Node>);
void print_ast(Node*);
std::unique_ptr<CompilationUnit> compile(Node*);

class CompilationUnit {
public:
	static void initialize();

	CompilationUnit();
	bool process(Node*);
	std::error_code dump(std::string, int);

	std::unique_ptr<llvm::LLVMContext> context;
	llvm::IRBuilder<> builder;
	std::unique_ptr<llvm::Module> module;
};

struct cmd_line_args
{
	int printflag;
	int lexflag;
	int printir;
	int optlevel;
	char* filename;
};

#endif // CCC_COMPILER_HPP_INCLUDED

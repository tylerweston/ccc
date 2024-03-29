/*
 * ccc - based on lab work & project for UofT course compilers & interpreters 
 * By: Tyler Weston
 * testing suites written by Stephen Keith
 */

#include "headers/compiler.hpp"
#include "headers/nodes.hpp"
#include "headers/preprocess.hpp"
#include "headers/main.hpp"
#include "headers/argsparse.hpp"
#include "headers/consolecolors.hpp"
#include <string>
#include <unistd.h>

/* TODO:
Don't need or implement later:
auto		- don't bother, auto is useless
volatile	- declare memory may change even if it looks like it won't.
inline		- once macros are implemented, use a similar strategy? Prog. will function same whether function is inlined or not.
register	- store variable in a register instead of on the stack.
restrict	- used with pointers, for optimization stuff, lets compiler know that pointer is the only way to access that memory location.

extern		- declare something without defining it or allocing memory for it. 

Difficult? 
sizeof(type)		- return size of type
struct
typedef
union

Switch keywords:
switch
case
default

Type keywords:
long
short
static
unsigned
signed

Control flow keywords:
do
else
enum
goto

const		- for optimizations, declare that variable is constant. ie, once it has been defined, we can replace all instances of it with the value.


% operator -needs codegen

Bitwise operators:
^ xor operator
& and operator
| or operator
~ bitwise not operator (Needs to be supported by unary node)
<< left shift
>> right shift
- these all need codegen


, evalute multiple expressions and return value of last one

Adding new keywords:
- tokens defined in parser.y and grammar is defined there
- token generation rules are in lexer.l


What other operators are there?

- bool isn't really part of the C language? remove support?
- built in true and false support?
- finalize double support, right now it is getting confused with floats
- This will probably mean there needs to be a way to promote and demote types
- 
*/


using namespace std::string_literals;

int main(int argc, char** argv) {
	std::cout << "cimple c compiler - ccc - Tyler Weston - 2020/2021\n";
 	cmd_line_args cmds;
	parse_commands(argc, argv, &cmds);
	// make sure we got a file
	if (!cmds.filename)
	{
		std::cout << "Must supply filename to compile\n";
		return 1;
	}

	// preprocessing
	std::cout << "Preprocessing file. " << cmds.filename << " -> " << cmds.filename << ".pp\n";
	preprocess* pp = new preprocess();
	pp->preprocess_file(cmds.filename, cmds.filename + ".pp"s);

	// show our lexing if we get the lexing flag
	if (cmds.lexflag)
	{
		lex(cmds.filename + ".pp"s);
	}


	// parsing
	std::cout << "Parsing file " << cmds.filename << ".pp\n";
	std::unique_ptr<Node> root;
	int ret = parse(cmds.filename + ".pp"s, root);
	if (ret != 0) {
		return 1;
	}

	// semantic analysis
	std::cout << "Performing semantic analysis\n";
	if (!verify_ast(root.get())) {
		std::cout << "Semantic analysis failed.\n";
		return 1;
	}
	if (cmds.printflag)
	{
		std::cout << "Generated AST (pre-optimization):\n";
		print_ast(root.get());
	}

	// optimization
	if (cmds.optlevel == 1)
	{
		std::cout << "Optimizing AST\n";
		root = optimize(std::move(root));
	}
	if (cmds.printflag)
	{
		std::cout << "Generated AST (post-optimization):\n";
		print_ast(root.get());
	}

	std::cout << "Generating IR\n";
	std::unique_ptr<CompilationUnit> u = compile(root.get());
	if (u == nullptr)
	{
		std::cout << "[" << RED << "ERROR" << RESET << "] Error generating llvm IR\n";
		return 1;
	}
	u->dump(cmds.filename + ".ll"s, cmds.printir);	// this will be the generated code

	// any cleanup happens here
	if (!cmds.keep_pp)
		pp->clean_preprocess_file(cmds.filename + ".pp"s);

	std::cout << "[" << GREEN << "SUCCESS" << RESET << "] All done!\n";
	return 0;
}

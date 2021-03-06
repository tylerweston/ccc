/*
 * ccc - based on lab work for UofT course compilers & interpreters 
 * By: Tyler Weston
 * testing suites written by Stephen Keith
 */

#include "compiler.hpp"
#include "nodes.hpp"
#include "preprocess.hpp"
#include <string>
#include <unistd.h>

using namespace std::string_literals;
int parse_commands(int, char**, cmd_line_args*);
static void usage();

int main(int argc, char** argv) {
	std::cout << "cimple c compiler - Tyler Weston - 2020\n";
 	cmd_line_args cmds;
	parse_commands(argc, argv, &cmds);
	// make sure we got a file
	if (!cmds.filename)
	{
		// TODO: make this make more sense
		// printf("Must supply filename\n");
		std::cout << "Must supply filename to compile\n";
		return 1;
	}

	// preprocessing
	std::cout << "Preprocessing file " << cmds.filename << "\n";
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
		std::cout << "Error generating llvm IR\n";
		return 1;
	}
	u->dump(cmds.filename + ".ll"s, cmds.printir);	// this will be the generated code

	std::cout << "All done!\n";
	return 0;
}

int parse_commands(int argc, char** argv, cmd_line_args* cmds)
{
	// init with defaults here
	cmds->printflag = 0;
	cmds->printir = 0;
	cmds->lexflag = 0;
	cmds->optlevel = 1;
	cmds->filename = NULL;
	int index;
	int optcode;

	opterr = 0;

	if (argc == 1)
	{
		usage();
		exit(0);
	}

	while ((optcode = getopt (argc, argv, "ialo:")) != -1)
	{
		switch (optcode)
		{
			case 'i':
				cmds->printir = 1;
				break;
			case 'a':
				// print
				cmds->printflag = 1;
				break;
			case 'l':
				// display lexing information
				cmds->lexflag = 1;
				break;
			case 'n':
				try 
				{
					cmds->optlevel = std::stoi(optarg);
				}
				catch (const std::invalid_argument& ia) 
				{
					std::cerr << "Invalid argument to -o: " << optarg << '\n';
				}
				break;
			case '?':
				// todo: fix this up
				if (optopt == 'o')
				{
					std::cerr << "Option -" << optopt << " requires an argument\n";
				}
				else
				{
					std::cerr << "Unknown option " << optopt << "\n";
				}
				return 1;
			default:
				exit(1);
		}
	}

	for (index = optind; index < argc; index++)
	{
		cmds->filename = argv[index];
		// todo: gather the rest of argv to use as the arguments to our program
	}
	return 0;
}

static void usage() 
{
	std::cout  	<< "[usage] ccc <args> <file> [compiled program args]\n"
				<< " -o0	: Disable optimization\n"
				<< " -o1	: Basic optimizations (default)\n"
				<< " -l 	: Display lexer output\n"
				<< " -a 	: Display AST\n"
				<< " -i 	: Display generated IR\n";
}

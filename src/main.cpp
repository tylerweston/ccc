/*
 * UofT ECE 467
 * - 997438170, Tyler, Weston
 */

#include "compiler.hpp"
#include "nodes.hpp"
#include <string>
#include <cstdlib>
#include <cstdio>

using namespace std::string_literals;

static void usage() {
	printf("[usage] ece467c <lab> <file> [compiled program args]\n");
}

int main(int argc, char** argv) {
	if (argc <= 2) {
		usage();
		return 1;
	}
	long lab { std::strtol(argv[1], nullptr, 10) };
	if (lab == 1) {
		printf("[config] lab 1.\n");
		int ret = lex(argv[2]);
		return ret;
	} else if (lab == 2) {
		printf("[config] lab 2.\n");
		std::unique_ptr<Node> ignored;
		int ret = parse(argv[2], ignored);
		return ret;
	} else if (lab == 3) {
		printf("[config] lab 3.\n");
		std::unique_ptr<Node> root;
		int ret = parse(argv[2], root);
		if (ret != 0) {
			return 1;
		}
		if (!verify_ast(root.get())) {
			printf("[output] semantic analysis failed.\n");
			return 1;
		}
		root = optimize(std::move(root));
		print_ast(root.get());
		return 0;
	} else if (lab == 4) {
		printf("[config] lab 4.\n");
		std::unique_ptr<Node> root;
		int ret = parse(argv[2], root);
		if (ret != 0) {
			return 1;
		}
		if (!verify_ast(root.get())) {
			printf("[output] semantic analysis failed.\n");
			return 1;
		}
		root = optimize(std::move(root));
		std::unique_ptr<CompilationUnit> u = compile(root.get());
		if (u == nullptr)
		{
			printf("[error] failed to compile.\n");
			return 1;
		}
		u->dump(argv[2] + ".ll"s);	// this will be the generated code
		return 0; 	// u->run(argc - 2, argv + 2); changes as per Lab 4 update.
	} else {
		printf("[error] invalid lab.\n");
		return 1;
	}
	return 0;
}

#include "compiler.hpp"
#include <cstdlib>
#include <cstdio>

static void usage() {
	printf("[usage] ece467c <lab> <file>\n");
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
		int ret = parse(argv[2]);
		return ret;
	} else if (lab == 3) {
		printf("[config] lab 3.\n");
		return 0;
	} else if (lab == 4) {
		printf("[config] lab 4.\n");
		return 0;
	} else {
		printf("[error] invalid lab.\n");
		return 1;
	}
	return 0;
}

#include "compiler.hpp"
#include <cstdio>

static void usage() {
	printf("[usage] ece467c <lab> <file>\n");
}

int main(int argc, char** argv) {
	if (argc <= 1) {
		usage();
		return 1;
	}
	compile(argv[1]);
	return 0;
}

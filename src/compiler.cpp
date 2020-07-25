#include "lexer.hpp"
#include "parser.hpp"
#include "nodes.hpp"
#include <cstdio>

void compile(char const* path) {
	(void) path;

	FILE* in = fopen(path, "r");
	if (in == nullptr) {
		return;
	}

	yyscan_t lexer;
	yylex_init_extra(1, &lexer);

	yyset_in(in, lexer);

	Node* root = nullptr;

	yy::parser p(lexer, &root);
	p.parse();

	yylex_destroy(lexer);
	fclose(in);
}

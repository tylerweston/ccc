#include "lexer.hpp"
#include "parser.hpp"
#include "nodes.hpp"
#include <cstdio>
#include <cassert>
#include <cstring>

int lex(char const* path) {
	FILE* in = fopen(path, "r");
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
		printf("[output] lexer got symbol: %s.\n", s.name());
		if (s.kind() == yy::parser::symbol_kind_type::S_YYEOF) {
			break;
		}
	}

	yylex_destroy(lexer);
	fclose(in);

	return 0;
}

int parse(char const* path) {
	FILE* in = fopen(path, "r");
	if (in == nullptr) {
		printf("[error] unable to open file: %s\n", std::strerror(errno));
		return 1;
	}

	yyscan_t lexer;
	yylex_init_extra(1, &lexer);

	yyset_in(in, lexer);

	Node* root = nullptr;

	yy::parser p(lexer, &root);
	p.parse();

	yylex_destroy(lexer);
	fclose(in);

	return 0;
}

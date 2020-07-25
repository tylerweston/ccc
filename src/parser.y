%code requires {

// this will be added to your parser.hpp file

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

class Node;

}

%code {

// this will be added to your parser.cpp file

#include "lexer.hpp"
#include "nodes.hpp"

static yy::parser::symbol_type yylex(yyscan_t);

}

%require "3.6"
%language "c++"
%locations
%param { yyscan_t lexer }
%parse-param { Node** root }
%verbose
%define api.value.type variant
%define api.token.constructor
%define parse.trace
%define parse.assert

%token <std::string> HI

/*
%type <Node*> root
*/

%start root

%%

root
	: declarations
	;

declarations
	: declarations decl
	| %empty
	;

decl
	: HI { printf("decl: %s\n", $1.c_str()); }
	;

%%

yy::parser::symbol_type yylex(yyscan_t lexer) {
	yy::parser::symbol_type s;
	int x = yylex(&s, nullptr, lexer);
	assert(x == 1);
	printf("[debug] got symbol %d (%s).\n", s.kind(), s.name());
	return s;
}

void yy::parser::error(location_type const& loc, std::string const& msg) {
	std::cout << "[error] parser error at " << loc << ": " << msg << ".\n";
}

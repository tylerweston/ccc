%{

// this will be added to the top of your lexer.c file

#include "bridge.hpp"

#define GET_COLUMN() yyget_extra(yyscanner)
#define SET_COLUMN(x) yyset_extra((x), yyscanner)
#define INC_COLUMN(x) yyset_extra(GET_COLUMN() + (x), yyscanner)

#define LOC(x) (yy::parser::location_type { nullptr, yylineno, (x) })

#define TOK(t, x, ...) \
	make_symbol(yylval, yy::parser::make_ ##t (__VA_ARGS__ __VA_OPT__(,) LOC(x)))

#define yyterminate() return TOK(YYEOF, GET_COLUMN())

static int make_symbol(YYSTYPE*, YYSTYPE);

%}

/* see https://westes.github.io/flex/manual/Scanner-Options.html */

%option header-file="lexer.h"

%option yylineno
%option noyywrap

%option bison-bridge
%option bison-locations
%option reentrant
%option nounput
%option extra-type="int"

%option debug
%option nodefault
%option verbose
%option warn

WS [ \r\n\t]+
DIGIT [0-9]

%%

{WS} { /* pass */ }

{DIGIT} { int x = GET_COLUMN(); INC_COLUMN(2); return TOK(HI, x, std::string { yytext, static_cast<size_t>(yyleng) }); }

. { printf("[error] invalid token.\n"); return TOK(YYUNDEF, GET_COLUMN()); }

%%

int make_symbol(YYSTYPE* val, YYSTYPE sym) {
	val->move(sym);
	return 1;
}

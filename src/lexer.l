%{

// this will be added to the top of your lexer.c file

#include "bridge.hpp"
#include <math.h>

#define GET_COLUMN() yyget_extra(yyscanner)
#define SET_COLUMN(x) yyset_extra((x), yyscanner)
#define INC_COLUMN(x) yyset_extra(GET_COLUMN() + (x), yyscanner)

#define LOC() (yy::parser::location_type { nullptr, yylineno, GET_COLUMN() })

#define TOK(t, ...) \
	make_symbol(yylval, yy::parser::make_ ##t (__VA_ARGS__ __VA_OPT__(,) LOC()))

#define yyterminate() return TOK(YYEOF)

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
LETTER [a-zA-Z]

%%

{WS} { /* pass */ }

{LETTER}|"-"({LETTER}|{DIGIT}|"_")+ { return TOK(TOK_IDENTIFIER, std::string { yytext, static_cast<size_t>(yyleng)}); }
{DIGIT}+ { return TOK(TOK_INTEGER_LITERAL, atoi(yytext)); }
{DIGIT}+"."{DIGIT}+ { return TOK(TOK_FLOAT_LITERAL, atof(yytext)); }
true { return TOK(TOK_BOOLEAN_LITERAL, true); }
false { return TOK(TOK_BOOLEAN_LITERAL, false); }
"(" { return TOK(TOK_LPAREN); }
")" { return TOK(TOK_RPAREN); }
"{" { return TOK(TOK_LBRACE); }
"}" { return TOK(TOK_RBRACE); }
"==" { return TOK(TOK_EQ); }
"!=" { return TOK(TOK_NE); }
"<" { return TOK(TOK_LT); }
">" { return TOK(TOK_GT); }
"<=" { return TOK(TOK_LE); }
">=" { return TOK(TOK_GE); }
"+" { return TOK(TOK_PLUS); }
"-" { return TOK(TOK_MINUS); }
"*" { return TOK(TOK_STAR); }
"/" { return TOK(TOK_SLASH); }
"&&" { return TOK(TOK_LOG_AND); }
"||" { return TOK(TOK_LOG_OR); }
"if" { return TOK(TOK_IF); }
"while" { return TOK(TOK_WHILE); }
"for" { return TOK(TOK_FOR); }
"break" { return TOK(TOK_BREAK); }
"continue" { return TOK(TOK_CONTINUE); }
"return" { return TOK(TOK_RETURN); }
"," { return TOK(TOK_COMMA); }
";" {return TOK(TOK_SEMICOLON); }
":" { return TOK(TOK_COLON); }
"?" { return TOK(TOK_QUESTION_MARK); }


{DIGIT} { INC_COLUMN(2); return TOK(HI, std::string { yytext, static_cast<size_t>(yyleng) }); }

. { printf("[error] invalid token.\n"); return TOK(YYUNDEF); }

%%

int make_symbol(YYSTYPE* val, YYSTYPE sym) {
	val->move(sym);
	return 1;
}

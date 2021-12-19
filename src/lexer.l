%{

// this will be added to the top of your lexer.c file

#include "bridge.hpp"
#include <string>
#include "nodes.hpp"	// so we can use our enum classes

#define GET_COLUMN() yyget_extra(yyscanner)
#define SET_COLUMN(x) yyset_extra((x), yyscanner)
#define INC_COLUMN(x) yyset_extra(GET_COLUMN() + (x), yyscanner)

#define LOC(x) (yy::parser::location_type { nullptr, yylineno, (x) })

#define TOK(t, x, ...) \
	make_symbol(yylval, yy::parser::make_ ##t (__VA_ARGS__ __VA_OPT__(,) LOC(x)))

// macro to take care of column tracking for us, take optional extra data
#define GEN_TOK(tok_name, ...) \
do { \
	int x = GET_COLUMN(); \
	INC_COLUMN(yyleng); \
	return TOK(tok_name, x __VA_OPT__(,) __VA_ARGS__); \
} while(0);

#define yyterminate() return TOK(YYEOF, GET_COLUMN())

static int make_symbol(YYSTYPE*, YYSTYPE);

// TODO: How can we use 
char get_char_from_yytext(std::string yystring)
{
	// char is either of form '\*' or '*' where * is an arbitrary-ish char
	if (yystring[1] == '\\')
	{
		if (yystring.length()==3)
		{
			//todo: throw a better error here
			printf("Error! Cannot parse char\n");
			exit(1);
		}
		switch(yystring[2])
		{
			case 'a':
				return '\a';
			case 'b':
				return '\b';
			case 'f':
				return '\f';
			case 'n':
				return '\n';
			case 'r':
				return '\r';
			case 't':
				return '\t';
			case 'v':
				return '\v';
			case '0':
				return '\0';
			default:
				printf("Error! Bad escape character\n");
				exit(1);
		}
	}
	else
	{
		return yystring[1];
	}
}

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

WS [ \r\t]+
DIGIT [0-9]
LETTER [a-zA-Z]

REGEX_IDENTIFIER ({LETTER}|"_")({LETTER}|{DIGIT}|"_")*

REGEX_FLOAT_EXPONENT ([eEpP][+-]?{DIGIT}+)
REGEX_FLOAT1 ({DIGIT}+{REGEX_FLOAT_EXPONENT})
REGEX_FLOAT2 ({DIGIT}+"."{REGEX_FLOAT_EXPONENT}?)
REGEX_FLOAT3 ({DIGIT}*"."{DIGIT}+{REGEX_FLOAT_EXPONENT}?)
REGEX_FLOAT {REGEX_FLOAT1}|{REGEX_FLOAT2}|{REGEX_FLOAT3}

REGEX_INTEGER {DIGIT}+

/* which one of these ?! */
REGEX_CHAR (\'(\\[abfnrtv0\\\'\"\?]|[a-zA-Z0-9])\')

%%

"\n" { SET_COLUMN(1); }
{WS} { INC_COLUMN(yyleng); }

if { GEN_TOK(TOK_IF); }
while { GEN_TOK(TOK_WHILE); }
for { GEN_TOK(TOK_FOR); }
break { GEN_TOK(TOK_BREAK); }
continue { GEN_TOK(TOK_CONTINUE); }
return { GEN_TOK(TOK_RETURN); }
const { GEN_TOK(TOK_CONST); }

int { GEN_TOK(TOK_TYPE, TypeName::tInt); }
float { GEN_TOK(TOK_TYPE, TypeName::tFloat); }
bool { GEN_TOK(TOK_TYPE, TypeName::tBool); }
void { GEN_TOK(TOK_TYPE, TypeName::tVoid); }
char { GEN_TOK(TOK_TYPE, TypeName::tChar); }
double { GEN_TOK(TOK_TYPE, TypeName::tDouble); }
short { GEN_TOK(TOK_TYPE, TypeName::tShort); }
long { GEN_TOK(TOK_TYPE, TypeName::tLong); }

true { GEN_TOK(TOK_TRUE); }
false { GEN_TOK(TOK_FALSE); }

{REGEX_IDENTIFIER} { GEN_TOK(TOK_IDENTIFIER, std::string { yytext, static_cast<size_t>(yyleng)}); }
{REGEX_FLOAT} { GEN_TOK(TOK_FLOAT, std::stof(yytext)); }			
{REGEX_INTEGER} { GEN_TOK(TOK_INTEGER, std::stoi(yytext)); }

"\(" { GEN_TOK(TOK_LPAREN); }
"\)" { GEN_TOK(TOK_RPAREN); }
"{" { GEN_TOK(TOK_LBRACE); }
"}" { GEN_TOK(TOK_RBRACE); }

"!=" { GEN_TOK(TOK_NE); }
"==" { GEN_TOK(TOK_EQ); }
"<" { GEN_TOK(TOK_LT); }
">" { GEN_TOK(TOK_GT); }
"<=" { GEN_TOK(TOK_LE); }
">=" { GEN_TOK(TOK_GE); }

"+=" {GEN_TOK(TOK_PLUS_ASSIGN); }
"-=" {GEN_TOK(TOK_MINUS_ASSIGN); }
"*=" {GEN_TOK(TOK_STAR_ASSIGN); }
"/=" {GEN_TOK(TOK_SLASH_ASSIGN); }

"\+" { GEN_TOK(TOK_PLUS); }
"-" { GEN_TOK(TOK_MINUS); }
"\*" { GEN_TOK(TOK_STAR); }
"/" { GEN_TOK(TOK_SLASH); }
"%" { GEN_TOK(TOK_MOD); }

"&&" { GEN_TOK(TOK_LOG_AND); }
"||" { GEN_TOK(TOK_LOG_OR); }

"&" { GEN_TOK(TOK_BIT_AND); }
"|" { GEN_TOK(TOK_BIT_OR); }
"^" { GEN_TOK(TOK_BIT_XOR); }
"~" { GEN_TOK(TOK_BIT_NOT); }

"<<" { GEN_TOK(TOK_LEFT_SHIFT); }
">>" { GEN_TOK(TOK_RIGHT_SHIFT); }

"," { GEN_TOK(TOK_COMMA); }
";" {GEN_TOK(TOK_SEMICOLON); }
":" { GEN_TOK(TOK_COLON); }
"\?" { GEN_TOK(TOK_QUESTION_MARK); }
"=" { GEN_TOK(TOK_ASSIGN); }

{REGEX_CHAR} { GEN_TOK(TOK_CHAR, get_char_from_yytext(yytext));}	/* need extract char function here */

. { printf("[error] invalid token.\n"); return TOK(YYUNDEF, GET_COLUMN()); }

%%

int make_symbol(YYSTYPE* val, YYSTYPE sym) {
	val->move(sym);
	return 1;
}

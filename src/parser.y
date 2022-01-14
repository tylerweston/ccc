%code requires {

// this will be added to your parser.hpp file

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

#include "headers/nodes.hpp"
#include "headers/common.hpp"
#include <memory>

class Node;

}

%code {

// this will be added to your parser.cpp file

#include "headers/lexer.hpp"

static yy::parser::symbol_type yylex(yyscan_t);

template <typename T, typename... Args> static std::unique_ptr<T> make_node(yy::parser::location_type const&, Args&&...);

}

/* see https://www.gnu.org/software/bison/manual/html_node/Declarations.html */

%require "3.6"
%language "c++"
%locations
%param { yyscan_t lexer }
%parse-param { std::unique_ptr<Node>& root }
%verbose
%define api.value.type variant
%define api.token.constructor
%define api.value.automove
%define parse.trace
%define parse.assert

/* Define our tokens here */

// value holding tokens
%token <std::string> TOK_IDENTIFIER
%token <char> TOK_CHAR
%token <int> TOK_INTEGER
%token <float> TOK_FLOAT
%token <double> TOK_DOUBLE
%token <short> TOK_SHORT
%token <long> TOK_LONG
%token <TypeName> TOK_TYPE

// bool literal tokens
%token TOK_TRUE TOK_FALSE

// ternary operator
%right TOK_COLON TOK_QUESTION_MARK

// operators
%left TOK_LOG_OR
%left TOK_LOG_AND

%left TOK_BIT_XOR TOK_BIT_AND TOK_BIT_OR
%left TOK_BIT_NOT

%left TOK_LEFT_SHIFT TOK_RIGHT_SHIFT

%left TOK_EQ TOK_NE 
%left TOK_LT TOK_GT TOK_LE TOK_GE

%left TOK_PLUS TOK_MINUS
%left TOK_STAR TOK_SLASH
%left TOK_MOD

%right TOK_PLUS_ASSIGN TOK_MINUS_ASSIGN
%right TOK_STAR_ASSIGN TOK_SLASH_ASSIGN

// keywords
%token TOK_IF TOK_WHILE TOK_FOR TOK_BREAK TOK_CONTINUE TOK_RETURN
%token TOK_CONST

// types
%token TOK_TYPE_INT TOK_TYPE_FLOAT TOK_TYPE_BOOL TOK_TYPE_VOID
%token TOK_TYPE_CHAR TOK_TYPE_DOUBLE TOK_TYPE_SHORT TOK_TYPE_LONG

// symbols
%token TOK_LPAREN TOK_RPAREN
%token TOK_LBRACE TOK_RBRACE
%token TOK_SINGLE_QUOTE


%token TOK_COMMA TOK_SEMICOLON 
%right TOK_ASSIGN

// dummy precedence tags
%left HI_PREC

// Easier way to do this?

%type <std::unique_ptr<Node>> root

%type <std::vector<std::unique_ptr<Node>>> function_list
%type <std::unique_ptr<Node>> function
%type <std::unique_ptr<FuncDeclNode>> function_decl
%type <std::unique_ptr<Node>> function_defn

%type <std::vector<std::unique_ptr<DeclarationNode>>> parameter_list
%type <std::vector<std::unique_ptr<DeclarationNode>>> non_empty_parameter_list

%type <std::unique_ptr<Node>> block
%type <std::vector<std::unique_ptr<Node>>> suite

%type <std::unique_ptr<DeclarationNode>> declaration
%type <std::unique_ptr<StatementNode>> statement
%type <std::unique_ptr<StatementNode>> single_statement
%type <std::unique_ptr<ExpressionNode>> maybe_expression
%type <std::unique_ptr<ExpressionNode>> expression
%type <std::unique_ptr<StatementNode>> compound_statement
%type <std::unique_ptr<Node>> maybe_single_statement
%type <std::unique_ptr<ExpressionNode>> ternary_expression
%type <std::unique_ptr<ExpressionNode>> unary_expression
%type <std::unique_ptr<ExpressionNode>> binary_expression

// %type <std::unique_ptr<ExpressionNode>> logical_expression

%type <std::unique_ptr<ExpressionNode>> relational_expression

/* %type <std::unique_ptr<Node>> unary_op */	// we remove this since we only have a single unary operator

%type <std::unique_ptr<ExpressionNode>> cast_expression
%type <std::unique_ptr<ExpressionNode>> function_call

%type <std::vector<std::unique_ptr<ExpressionNode>>> func_args
%type <std::vector<std::unique_ptr<ExpressionNode>>> non_empty_func_args

%type <AugmentedAssignOps> augmented_assign
// %type <BinaryOps> binary_op
// %type <RelationalOps> relational_op
%type <TypeName> type
%type <std::string> name


%start root

%%

/* Grammar rules */

// nullptr is pretty much the better c++ version of null but is always a pointer

root
	: function_list	
		{ this->root = make_node<RootNode>(@$, $1); }
	;

/* A function list is one or more functions */
function_list
	: function 						
		{ 		
			$$ = std::vector<std::unique_ptr<Node>>{}; 
			$$.push_back($1);
		}
	| function_list function 		
		{
			$$ = $1; 
			$$.push_back($2);
		}
	;

function
	: function_decl TOK_SEMICOLON	
		{ $$ = $1;}
	| function_defn					
		{ $$ = $1;}
	;

function_decl
	: type name TOK_LPAREN parameter_list TOK_RPAREN	
		{ $$ = make_node<FuncDeclNode>(@$, $1, $2, $4); }
	;

function_defn
	: function_decl block 
		{ $$ = make_node<FuncDefnNode>(@$, $1, $2); }
	;

type
	: TOK_TYPE 
		{ $$ = $1; }
	;

name
	: TOK_IDENTIFIER 
		{ $$ = $1; }
	;

/* To handle one or more comma delimited lists, we need to add an extra rule */

parameter_list
	: %empty 
		{ 
			$$ = std::vector<std::unique_ptr<DeclarationNode>>{}; 
		}
	| non_empty_parameter_list 
		{ 
			$$ = $1; 
		}
	;

non_empty_parameter_list
	: declaration
		{ 	
			$$ = std::vector<std::unique_ptr<DeclarationNode>>{}; 
			$$.push_back($1);
		}
	| non_empty_parameter_list TOK_COMMA declaration
		{ 
			$$ = $1; 
			$$.push_back($3); 
		}

block
	: TOK_LBRACE suite TOK_RBRACE 
		{ $$ = make_node<BlockNode>(@$, $2); }
	;

suite								
	: %empty 
		{ $$ = std::vector<std::unique_ptr<Node>>{}; }
	| suite statement 				
		{ 
			$$ = $1; 
			$$.push_back($2); 
		}
	;

declaration
	: type name 
		{ $$ = make_node<DeclarationNode>(@$, $1, $2, false); }
	| TOK_CONST type name
		{ $$ = make_node<DeclarationNode>(@$, $2, $3, true); }
	;

statement
	: single_statement TOK_SEMICOLON 
		{ $$ = $1; }
	| compound_statement 
		{ $$ = $1; }
	;

single_statement
	: declaration TOK_ASSIGN expression 
		{ $$ = make_node<DeclAndAssignNode>(@$, $1, $3); }
	| name TOK_ASSIGN expression 
		{ $$ = make_node<AssignmentNode>(@$, $1, $3); }
	| name augmented_assign expression 
		{ $$ = make_node<AugmentedAssignmentNode>(@$, $2, $1, $3); }
	| TOK_DOUBLE 
		{ $$ = make_node<BreakNode>(@$); }
	| TOK_CONTINUE 
		{ $$ = make_node<ContinueNode>(@$); }
	| TOK_RETURN 
		{ $$ = make_node<ReturnNode>(@$, nullptr); }
	| TOK_RETURN expression 
		{ $$ = make_node<ReturnNode>(@$, $2); }
	| expression 
		{ $$ = make_node<ExpressionStatementNode>(@$, $1); }
	;

expression
	: TOK_TRUE 
		{ $$ = make_node<ConstantBoolNode>(@$, true); }
	| TOK_FALSE 
		{ $$ = make_node<ConstantBoolNode>(@$, false); }
	| TOK_INTEGER 							
		{ $$ = make_node<ConstantIntNode>(@$, $1); }
	| TOK_FLOAT 							
		{ $$ = make_node<ConstantFloatNode>(@$, $1); }
	| TOK_CHAR
		{ $$ = make_node<ConstantCharNode>(@$, $1); }
	| TOK_DOUBLE
		{ $$ = make_node<ConstantDoubleNode>(@$, $1); }
	| name									
		{ $$ = make_node<VariableNode>(@$, $1); }
	| ternary_expression
		{ $$ = $1; }
	| binary_expression
		{ $$ = $1; }
	| unary_expression
		{ $$ = $1; }
	| relational_expression
		{ $$ = $1; }
	| cast_expression
		{ $$ = $1; }
	| function_call
		{ $$ = $1; }
	| TOK_LPAREN expression TOK_RPAREN
		{ $$ = $2; }
	;

compound_statement
	: TOK_IF TOK_LPAREN expression TOK_RPAREN block 	
		{ $$ = make_node<IfNode>(@$, $3, $5); }
	| TOK_FOR TOK_LPAREN maybe_single_statement TOK_SEMICOLON maybe_expression TOK_SEMICOLON maybe_single_statement TOK_RPAREN block 
		{ $$ = make_node<ForNode>(@$, $3, $5, $7, $9); }
	| TOK_WHILE TOK_LPAREN expression TOK_RPAREN block 	
		{ $$ = make_node<WhileNode>(@$, $3, $5); }
	; 

maybe_single_statement
	: %empty 
		{ $$ = nullptr; }
	| single_statement 						
		{ $$ = $1; }
	;

maybe_expression
	: %empty
		{ $$ = nullptr; }
	| expression
		{ $$ = $1; }
	;

unary_expression
	: TOK_MINUS expression %prec HI_PREC 		
		{ $$ = make_node<UnaryNode>(@$, UnaryOps::Minus, $2); }	
	| TOK_BIT_NOT expression %prec HI_PREC 		
		{ $$ = make_node<UnaryNode>(@$, UnaryOps::Not, $2); }
	;

binary_expression
	: expression TOK_PLUS expression 	{ $$ = make_node<BinaryOpNode>(@$, BinaryOps::Plus, $1, $3); }
	| expression TOK_MINUS expression 	{ $$ = make_node<BinaryOpNode>(@$, BinaryOps::Minus, $1, $3); }
	| expression TOK_STAR expression 	{ $$ = make_node<BinaryOpNode>(@$, BinaryOps::Star, $1, $3); }
	| expression TOK_SLASH expression 	{ $$ = make_node<BinaryOpNode>(@$, BinaryOps::Slash, $1, $3); }
	| expression TOK_LOG_AND expression { $$ = make_node<LogicalOpNode>(@$, BinaryOps::LogAnd, $1, $3); }
	| expression TOK_LOG_OR expression 	{ $$ = make_node<LogicalOpNode>(@$, BinaryOps::LogOr, $1, $3); }
	| expression TOK_MOD expression		{ $$ = make_node<BinaryOpNode>(@$, BinaryOps::Mod, $1, $3); }
	| expression TOK_BIT_AND expression { $$ = make_node<BinaryOpNode>(@$, BinaryOps::BitAnd, $1, $3); }
	| expression TOK_BIT_OR expression 	{ $$ = make_node<BinaryOpNode>(@$, BinaryOps::BitOr, $1, $3); }
	| expression TOK_BIT_XOR expression { $$ = make_node<BinaryOpNode>(@$, BinaryOps::BitXor, $1, $3); }
	| expression TOK_LEFT_SHIFT expression { $$ = make_node<BinaryOpNode>(@$, BinaryOps::LeftShift, $1, $3); }
	| expression TOK_RIGHT_SHIFT expression { $$ = make_node<BinaryOpNode>(@$, BinaryOps::RightShift, $1, $3); }
	;

relational_expression
	: expression TOK_EQ expression 		{ $$ = make_node<RelationalOpNode>(@$, RelationalOps::Eq, $1, $3); }
	| expression TOK_NE expression 		{ $$ = make_node<RelationalOpNode>(@$, RelationalOps::Ne, $1, $3); }
	| expression TOK_LT expression 		{ $$ = make_node<RelationalOpNode>(@$, RelationalOps::Lt, $1, $3); }
	| expression TOK_GT expression 		{ $$ = make_node<RelationalOpNode>(@$, RelationalOps::Gt, $1, $3); }
	| expression TOK_LE expression 		{ $$ = make_node<RelationalOpNode>(@$, RelationalOps::Le, $1, $3); }
	| expression TOK_GE expression 		{ $$ = make_node<RelationalOpNode>(@$, RelationalOps::Ge, $1, $3); }
	;

ternary_expression
	: expression TOK_QUESTION_MARK expression TOK_COLON expression
		{ $$ = make_node<TernaryNode>(@$, $1, $3, $5); }
	;

cast_expression
	: TOK_LPAREN type TOK_RPAREN expression %prec HI_PREC
		{ $$ = make_node<CastExpressionNode>(@$, $2, $4); }
	;

function_call
	: name TOK_LPAREN func_args TOK_RPAREN 	
		{ $$ = make_node<FuncCallNode>(@$, $1, $3); }
	;

/* To handle one or more comma delimited lists, we need to add an extra rule */

func_args
	: %empty 
		{ 
			$$ = std::vector<std::unique_ptr<ExpressionNode>>{}; 
		}
	| non_empty_func_args 
		{ 
			$$ = $1; 
		}
	;

non_empty_func_args
	: expression
		{ 	
			$$ = std::vector<std::unique_ptr<ExpressionNode>>{}; 
			$$.push_back($1);
		}
	| non_empty_func_args TOK_COMMA expression 
		{ 
			$$ = $1; 
			$$.push_back($3); 
		}

augmented_assign
	: TOK_PLUS_ASSIGN		{ $$ = AugmentedAssignOps::PlusEq; }
	| TOK_MINUS_ASSIGN		{ $$ = AugmentedAssignOps::MinusEq; }
	| TOK_STAR_ASSIGN		{ $$ = AugmentedAssignOps::StarEq; }
	| TOK_SLASH_ASSIGN		{ $$ = AugmentedAssignOps::SlashEq; }

%%

yy::parser::symbol_type yylex(yyscan_t lexer) {
	yy::parser::symbol_type s;
	int x = yylex(&s, nullptr, lexer);
	assert(x == 1);
	return s;
}

void yy::parser::error(location_type const& loc, std::string const& msg) {
	std::cout << "[error] parser error at " << loc << ": " << msg << ".\n";
}

template <typename T, typename... Args> static std::unique_ptr<T> make_node(yy::parser::location_type const& loc, Args&&... args) {
	std::unique_ptr<T> n = std::make_unique<T>(std::forward<Args>(args)...);
	n->location = loc;
	return n;
}

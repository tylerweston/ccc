# CCC
Cimple C Compiler
Flex, Bison, and LLVM compiler for a subset of the C language.
Generates LLVM IR.

# Keywords
The compiler supports the following C keywords:  
* if
* while
* for
* break
* continue
* return
  
It supports the following types:  
* int
* float
* bool
* void

It supports the following operations:
* Addition (+, +=)
* Subtraction (-, -=)
* Multiplication (*, *=)
* Division (/, /=)
* Logical and (&&)
* Logical or (||)
* Ternary expression (condition ? true expression : false expression)
    
**ccc** consists of the following components:  
* Preprocessor
  * Removes comments from the original source code
* Lexer
  * Uses Flex to parse the preprocessed file into lexemes
* Parser
  * Uses Bison to parse the lexems following our grammar rules
  * Generates an Abstract Synta Tree that we can use for further analysis and transformations
* Verification
  * Traverses our AST and performs semantic analysis of the program to ensure correctness
  * Errors checked:
    * Existence of main function
    * Redeclaration of variables in the same scope
    * Type mismatch in variable assignment or function returns
    * Type mismatch in binary, logical, or relational operation
    * Use of undeclared variables or functions
    * Missing return value in non-void functions
    * Redefinition of function (Multiple declaration are OK!)
    * Type returned from function doesn't match declared type
    * If, for, while, and ternary conditions must evaluate to boolean
    * Can only cast between int and float
    
* Optimization
  * Any binary, unary, or relational operation whose operands are strictly constant will be simplified as much as possible
  * If statements with constant predicates will either be removed entirely if it always evaluates to false, or always executed if the condition always evaluates to true.
  * Ternary operations with a constant predicate will be replaced with either their true expression or false expression.
  * While statements with a constantly false conditions will be removed entirely.
  
* Code generation
  * Traverse the AST and emit the appropriate LLVM IR
  
* Command line options
  * -i will print the generated IR
  * -a will print the AST both pre- and post-optimization
  * -l will display the generated lexems and locations from Flex
  * -o0 will disable optimizations
  * -o1 will perform basic optimization (default)
  
* Requirements:
  * Bison 3.6.4
  * Cmake 3.18.2
  * gcc 10.2.0
  * flex 2.6.4
  * gmp 4.3.2
  * mpc 1.0.1
  * mpfr 3.1.0
  * llvm 10.0.1
 
* Installation  
Create a folder, say cccroot, install all of the above into cccroot/prefix, and  then clone ccc into cccroot/ccc. From there, you can run make in cccroot/ccc/src to build the project

#include "headers/preprocess.hpp"
#include "headers/consolecolors.hpp"
#include <iostream>
#include <fstream>
#include <string>

// private preprocessing functions
int clean_comments(std::ifstream& readfile, std::ofstream& writefile);
int check_valid_character(char c);
int check_all_valid_characters(std::ifstream& readfile);
int replace_simple_macros(std::ifstream& readfile, std::ofstream& writefile);
int import_header_files(std::ifstream& readfile, std::ofstream& writefile);
int replace_predefined_macros(std::ifstream& readfile, std::ofstream& writefile);

int replace_predefined_macros(std::ifstream& readfile, std::ofstream& writefile) 
{

// 	__FILE__
// This macro expands to the name of the current input file, in the form of a C string constant. The precise name returned is 
// the one that was specified in `#include' or as the input file name argument.

// __LINE__
// This macro expands to the current input line number, in the form of a decimal integer constant. While we call it a predefined macro, 
// it's a pretty strange macro, since its "definition" changes with each new line of source code.
// This and `__FILE__' are useful in generating an error message to report an inconsistency detected by the program; the message can 
// state the source line at which the inconsistency was detected. For example,

 	
// fprintf (stderr, "Internal error: "
//                  "negative string length "
//                  "%d at %s, line %d.",
//          length, __FILE__, __LINE__);

// A `#include' directive changes the expansions of `__FILE__' and `__LINE__' to correspond to the included file. At the end of that file, when 
// processing resumes on the input file that contained the `#include' directive, the expansions of `__FILE__' and `__LINE__' revert to the values 
// they had before the `#include' (but `__LINE__' is then incremented by one as processing moves to the line after the `#include').

// The expansions of both `__FILE__' and `__LINE__' are altered if a `#line' directive is used. See section 1.6 Combining Source Files.

// __DATE__
// This macro expands to a string constant that describes the date on which the preprocessor is being run. 
// The string constant contains eleven characters and looks like `"Feb 1 1996"'.

// __TIME__
// This macro expands to a string constant that describes the time at which the preprocessor is being run. 
// The string constant contains eight characters and looks like `"23:59:01"'.

// __STDC__
// This macro expands to the constant 1, to signify that this is ANSI Standard C. 
// (Whether that is actually true depends on what C compiler will operate on the output from the preprocessor.)
// On some hosts, system include files use a different convention, where `__STDC__' is normally 0, but is 1 
// if the user specifies strict conformance to the C Standard. The preprocessor follows the host convention when 
// processing system include files, but when processing user files it follows the usual GNU C convention.

// This macro is not defined if the `-traditional' option is used.

// __STDC_VERSION__
// This macro expands to the C Standard's version number, a long integer constant of the form `yyyymmL' where yyyy and mm 
// are the year and month of the Standard version. This signifies which version of the C Standard the preprocessor conforms to. 
// Like `__STDC__', whether this version number is accurate for the entire implementation depends on what C compiler will operate on 
// the output from the preprocessor. This macro is not defined if the `-traditional' option is used.
	return 0;
}

int import_header_files(std::ifstream& readfile, std::ofstream& writefile)
{
	// find all #include statements and do a textual replacement
	return 0;
}

int replace_simple_macros(std::ifstream& readfile, std::ofstream& writefile)
{
	// find all object macros (ie. #define OBJECT_NAME value) and do a textual replacement
	return 0;
}

int preprocess::clean_preprocess_file(const std::string infile)
{
	// remove infile and return whether it was successful
	return std::remove(infile.c_str());
}

int preprocess::preprocess_file(const std::string infile, const std::string outfile)
{
	// try to open read file
	std::ifstream readfile(infile);
	if (!readfile.is_open())
	{
		std::cout << "[" << RED << "error" << RESET << "] Problem reading file " << infile << "\n";
		return 1;
	}

	// try to open write file
	std::ofstream writefile;
	writefile.open(outfile);
	if (!writefile.is_open())
	{
		std::cout << "[" << RED << "error" << RESET << "] Problem opening file for writing " << outfile << "\n";
		return 1;
	}

	// here that each transform the file a little bit.
	// check for valid characters
	// Remove comments
	// Remove newlines
	// predefined macros
	// Include header files ie. #include <system headers> or #include "local headers"
	// user defined macros
	check_all_valid_characters(readfile);
	readfile.clear();                 // clear fail and eof bits
	readfile.seekg(0, std::ios::beg); // back to the start!
	clean_comments(readfile, writefile);
	return 0;
}

int check_valid_character(char c)
{
	// check if the character is valid
	// valid characters are:
	// a-z, A-Z, 0-9, _, $, #, @, %, ^, &, *, (, ), -, +, =, [, ], {, }, <, >, ., ;, :, ?, !, ', ", \, /
	// and space
	if (c == ' ' || c == '\n' || c == '\t' || 
		c == '\r' || c == '\v' || c == '\f')
	{
		return 1;
	}
	if (c >= 'a' && c <= 'z')
	{
		return 1;
	}
	if (c >= 'A' && c <= 'Z')
	{
		return 1;
	}
	if (c >= '0' && c <= '9')
	{
		return 1;
	}
	if (c == ',' || c == '_' || c == '$' || c == '#' || 
		c == '@' || c == '%' || c == '^' || c == '&' || 
		c == '*' || c == '(' || c == ')' || c == '-' || 
		c == '+' || c == '=' || c == '[' || c == ']' || 
		c == '{' || c == '}' || c == '<' || c == '>' || 
		c == '.' || c == ';' || c == ':' || c == '?' || 
		c == '!' || c == '|' || c == '\'' || c == '\"' || 
		c == '\\' || c == '/')
	{
		return 1;
	}
	// // TODO: This should print a location or something since we probably won't actually be able to print this character!
	// std::cout << "[" << RED << "error" << RESET << "] Invalid character: " << c << "\n";
	return 0;
}

int check_all_valid_characters(std::ifstream& readfile)
{
	// check if all characters are valid
	char c;
	while (readfile.get(c))
	{
		if (!check_valid_character(c))
		{
			std::cout << "[" << RED << "error" << RESET << "] Invalid character in source\n";
			return 0;
		}
	}
	return 1;
}

int clean_comments(std::ifstream& readfile, std::ofstream& writefile)
{
	// Remove single and multiline comments from a file
	
	std::string inputLine;
	std::string outputLine;

	// simple state machine to track the comment state
	// TODO: We need to ignore comments if we are inside a string literal!
	// ie. std::cout << "Something like /*this*/\n"; is not a comment!
	enum class preprocess_state
	{
		normal = 0,
		normal_seen_slash,
		in_block_comment,
		in_block_comment_seen_star,
		in_string_literal,
	};
	preprocess_state cur_state = preprocess_state::normal;
	bool breakCommentFlag = false;

	// read entire input file
	while (getline(readfile, inputLine))
	{
		outputLine.clear();
		for (char const &c: inputLine)
		{
			// comment checking state machine
			switch (cur_state)
			{
				case preprocess_state::in_string_literal:
					if (c == '\"')
					{
						cur_state = preprocess_state::normal;
					}
					break;
				case preprocess_state::normal:
					if (c == '\"')
					{
						cur_state = preprocess_state::in_string_literal;
					}
					if (c == '/')
					{
						cur_state = preprocess_state::normal_seen_slash;
					}
					else
					{
						outputLine.push_back(c);
					}
					break;

				case preprocess_state::normal_seen_slash:
					if (c == '*')
					{
						cur_state = preprocess_state::in_block_comment;
					}
					else if (c == '/')
					{
						cur_state = preprocess_state::normal;
						breakCommentFlag = true;
					}
					else
					{
						cur_state = preprocess_state::normal;
						outputLine.push_back('/');
						outputLine.push_back(c);
					}
					break;

				case preprocess_state::in_block_comment:
					if (c == '*')
					{
						cur_state = preprocess_state::in_block_comment_seen_star;
					}
					break;

				case preprocess_state::in_block_comment_seen_star:
					if (c == '/')
					{
						cur_state = preprocess_state::normal;
					}
					else
					{
						cur_state = preprocess_state::in_block_comment;
					}
					break;
			}

			if (breakCommentFlag)
			{
				breakCommentFlag = false;
				break;
			}
		}
		// write output to our file
		writefile << outputLine << "\n";
	}
	// TODO: Error check, what if we don't end a comment before end of file?
	if (cur_state == preprocess_state::in_string_literal)
	{
		std::cout << "[" << RED << "error" << RESET << "] Unterminated string literal\n";
		return 0;
	}
	if (cur_state != preprocess_state::normal)
	{
		std::cout << "[" << RED << "error" << RESET << "] Unterminated comment\n";
		return 0;
	}
	readfile.close();
	writefile.close();
	return 0;
}
#include "headers/preprocess.hpp"
#include <iostream>
#include <fstream>
#include <string>

// private preprocessing functions
int clean_comments(std::ifstream& readfile, std::ofstream& writefile);
int check_valid_character(char c);
int check_all_valid_characters(std::ifstream& readfile);

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
		std::cout << "Error reading file " << infile << "\n";
		return 1;
	}

	// try to open write file
	std::ofstream writefile;
	writefile.open(outfile);
	if (!writefile.is_open())
	{
		std::cout << "Error opening file for writing " << infile << "\n";
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
	std::cout << "Invalid character: " << c << "\n";
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
			std::cout << "Error! Invalid character in source\n";
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
	enum class preprocess_state
	{
		normal = 0,
		normal_seen_slash,
		in_block_comment,
		in_block_comment_seen_star
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
				case preprocess_state::normal:
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
	if (cur_state != preprocess_state::normal)
	{
		std::cout << "Error! Unterminated comment\n";
		return 0;
	}
	readfile.close();
	writefile.close();
	return 0;
}
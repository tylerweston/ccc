#include "argsparse.hpp"
#include <iostream>
#include <getopt.h>

int parse_commands(int argc, char** argv, cmd_line_args* cmds)
{
	// init with defaults here
	// cmds->printflag = 0;
	// cmds->printir = 0;
	// cmds->lexflag = 0;
	// cmds->keep_pp = 0;
	// cmds->optlevel = 1;
	// cmds->filename = NULL;

	int index;
	int optcode;

	opterr = 0;

	if (argc == 1)
	{
		usage();
		exit(0);
	}

	while ((optcode = getopt (argc, argv, "eialo:")) != -1)
	{
		switch (optcode)
		{
			case 'i':
				cmds->printir = 1;
				break;
			case 'a':
				// print
				cmds->printflag = 1;
				break;
			case 'l':
				// display lexing information
				cmds->lexflag = 1;
				break;
			case 'e':
				// don't erase preprocessed file
				cmds->keep_pp = 1;
				break;
			case 'n':
				try 
				{
					cmds->optlevel = std::stoi(optarg);
				}
				catch (const std::invalid_argument& ia) 
				{
					std::cerr << "Invalid argument to -o: " << optarg << '\n';
				}
				break;
			case '?':
				// todo: fix this up
				if (optopt == 'o')
				{
					std::cerr << "Option -" << optopt << " requires an argument\n";
				}
				else
				{
					std::cerr << "Unknown option " << optopt << "\n";
				}
				return 1;
			default:
				exit(1);
		}
	}

	for (index = optind; index < argc; index++)
	{
		cmds->filename = argv[index];
		// todo: gather the rest of argv to use as the arguments to our program
	}
	return 0;
}

void usage() 
{
	std::cout  	<< "[usage] ccc <args> <file> [compiled program args]\n"
				<< " -o0	: Disable optimization\n"
				<< " -o1	: Basic optimizations (default)\n"
				<< " -l 	: Display lexer output\n"
				<< " -a 	: Display AST\n"
				<< " -i 	: Display generated IR\n"
				<< " -e     : Keep preprocessed file (as filename.pp)\n";
}

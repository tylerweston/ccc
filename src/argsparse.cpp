#include "argsparse.hpp"
#include <iostream>
#include <getopt.h>

int parse_commands(int argc, char** argv, cmd_line_args* cmds)
{
	int index;
	int optcode;

	opterr = 0;

	if (argc == 1)
	{
		usage();
		exit(0);
	}

    const struct option longopts[] = {
        {"print-lex", no_argument, 0, 'l'},
        {"print-ir", no_argument, 0, 'i'},
        {"print-ast", no_argument, 0, 'a'},
        {"print-tokens", no_argument, 0, 'l'},
        // {"print-pp", no_argument, 0, 'e'},
        {"optimization-level", required_argument, 0, 'o'},
        {"keep-preprocessed", no_argument, 0, 'e'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    while ((optcode = getopt_long(argc, argv, "eialo:hv", longopts, &index)) != -1)
    {
        switch (optcode)
        {
            case 'v':
                version_info();
                exit(0);
            case 'h':
                usage();
                exit(0);
            case 'a':
                cmds->printflag = 1;
                break;
            case 'i':
                cmds->printir = 1;
                break;
            case 'l':
                cmds->lexflag = 1;
                break;
            case 'p':
                cmds->printflag = 1;
                break;
            case 'o':
                cmds->optlevel = atoi(optarg);
                break;
            case 'e':
                cmds->keep_pp = 1;
                break;
            case '?':
                if (optopt == 'o')
                {
                    std::cerr << "Option -" << optopt << " requires an argument." << std::endl;
                }
                else if (isprint(optopt))
                {
                    std::cerr << "Unknown option -" << optopt << std::endl;
                }
                else
                {
                    std::cerr << "Unknown option character " << optopt << std::endl;
                }
                return 1;
            default:
                abort();
        }
    }

	for (index = optind; index < argc; index++)
	{
		cmds->filename = argv[index];
		// todo: gather the rest of argv to use as the arguments to our program?
        // or should we be able to build multiple programs probably? ie strcat it onto filename?
	}
	return 0;
}

void usage() 
{
	std::cout  	<< "[usage] ccc <args> <file>\n"
				<< " -o0\t--optimization-level 0\t\t: Disable optimization\n"
				<< " -o1\t--optimization-level 1\t\t: Basic optimizations (default)\n"
				<< " -l\t--print-lex\t\t\t: Display lexer output\n"
				<< " -a\t--print-ast\t\t\t: Display AST\n"
				<< " -i\t--print-ir\t\t\t: Display generated IR\n"
				<< " -e\t--keep-preprocessed\t\t: Keep preprocessed file (as filename.pp)\n"
                << " -h\t--help\t\t\t\t: Display this help message\n"
                << " -v\t--version\t\t\t: Display version information\n";
}

void version_info()
{
    std::cout   << "ccc version 0.1\n"
                << "By Tyler Weston\n"
                << "2020/2021\n";
}

#ifndef CCC_ARGPARSE_HPP_INCLUDED
#define CCC_ARGPARSE_HPP_INCLUDED

struct cmd_line_args
{
	int printflag = 0;
	int lexflag = 0;
	int printir = 0;
	int optlevel = 1;
	int keep_pp = 0;
	char* filename = nullptr;
};

int parse_commands(int, char**, cmd_line_args*);
void usage();
void version_info();

#endif // CCC_ARGPARSE_HPP_INCLUDED
#ifndef CCC_MAIN_HPP_INCLUDED
#define CCC_MAIN_HPP_INCLUDED

struct cmd_line_args
{
	int printflag = 0;
	int lexflag = 0;
	int printir = 0;
	int optlevel = 1;
	int keep_pp = 0;
	char* filename = NULL;
};

#endif // CCC_MAIN_HPP_INCLUDED

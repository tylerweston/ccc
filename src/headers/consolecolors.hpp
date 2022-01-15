//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
// https://stackoverflow.com/questions/9158150/colored-output-in-c
#ifdef __unix__ || defined(__APPLE__) || defined(__MACH__)
// Should work on unix or mac based systems
#define RESET       "\033[0m"
#define BLACK       "\033[30m"              /* Black */
#define RED         "\033[31m"              /* Red */
#define GREEN       "\033[32m"              /* Green */
#define YELLOW      "\033[33m"              /* Yellow */
#define BLUE        "\033[34m"              /* Blue */
#define MAGENTA     "\033[35m"              /* Magenta */
#define CYAN        "\033[36m"              /* Cyan */
#define WHITE       "\033[37m"              /* White */
#define BOLDBLACK   "\033[1m\033[30m"       /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"       /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"       /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"       /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"       /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"       /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"       /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"       /* Bold White */
#elif defined(_WIN32) || defined(_WIN64)
// TODO: How to get console color in windows?
#define RESET       ""
#define BLACK       ""
#define RED         ""
#define GREEN       ""
#define YELLOW      ""
#define BLUE        ""
#define MAGENTA     ""
#define CYAN        ""
#define WHITE       ""
#define BOLDBLACK   ""
#define BOLDRED     ""
#define BOLDGREEN   ""
#define BOLDYELLOW  ""
#define BOLDBLUE    ""
#define BOLDMAGENTA ""
#define BOLDCYAN    ""
#define BOLDWHITE   ""
#endif
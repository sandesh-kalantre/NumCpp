#include <iostream>
#include <cstdio>

#include "parser.hpp"
using namespace std;

//definitions of the maps
std::map<std::string, Function> map_functions;
std::map<std::string, Number> map_variables;
std::map<std::string, Routine> map_routines;
std::map<std::string, ndArray> map_ndarrays;

//the global bools which are used when parsing fails
bool suppress_zero = false;
bool suppress_eval = false;

//global unsigned long which stores the print precison
unsigned long print_precision;

int main(int argc,char** argv)
{
    //default precision for the program
    mpfr_set_default_prec(DEFAULT_PRECISION);
    //default print precision
    print_precision = DEFAULT_PRINT_PRECISION;

    //command line arguments to the progran
    for(int i = 1; i < argc;i++)
    {
        try
        {
            if(argv[i][0] == '-' && argv[i][1] == 'p')
            {
                int prec = std::atoi(argv[++i]);
                if(prec < 10 || prec > 1000000)
                {
                    throw WRONG_PRECISION;
                }
                mpfr_set_default_prec(prec);
                i++;
            }
            if(argv[i][0] == '-' && argv[i][1] == 'r')
            {
                unsigned long print_prec = (unsigned)std::atoi(argv[++i]);
                if(print_prec < 6 || print_prec > 100000)
                {
                    throw WRONG_PRINT_PRECISION;
                }
                print_precision = print_prec;
                i++;
            }

        }
        catch(const char *str)
        {
            std::cout<<"Argument error : "<<str<<std::endl;
            return -1;
        }
    }

    //initialize the maps with standard data
    def_functions();
    def_variables();
    def_routines();

    //create a Parser object
    Parser parser;

    //the text to be declared at the start of the program
    std::cout<<"______"<<std::endl;
    std::cout<<"NumCpp"<<std::endl;
    std::cout<<"______"<<std::endl;
    std::cout<<
    "\nNumCpp is a numerical library with an inbuilt parser for input\n"
    "that can be used to do numerical analysis such as:\n"
    "-integration\n"
    "-differentiation\n"
    "-root finding and much more.\n\n"
    "Use help(functions) for a list of standard functions.\n"
    "Use help(constants) for a list of stored constants.\n"
    "Use help(routines) for a list of routines and help(routine_name) for help on a specifc routine.\n"
    "Use exit() to exit the program.\n"
    <<std::endl;

    //the loop that runs the program
    while(true)
    {
        parser = Parser();
        string expr;
        string::iterator it_expr;

        //the prompt
        cout<<">>>";
        getline(cin,expr);

        //break if a single ; on a line is entered marking the end of the program
        //or if "exit()" is entered
        //even "exit" is allowed
        if(expr == ";" || expr.compare("exit()") == 0 || cin.eof() || expr.compare("exit") == 0)
        {
            cout<<"Thank You.\n";
            return 0;
        }

        //to ensure that even if ; is forgotten at the end of input
        //the program still runs without an error
        expr += ";";

        it_expr = expr.begin();
        parser.parse(expr);
    }

    return 0;
}

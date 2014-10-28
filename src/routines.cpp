#include <iostream>
#include <string>
#include <vector>
using namespace std;

double diff1 (std::string function_name, double a)
{
    std::vector<double> arguments (1);
    arguments[0] = a;
    double h = 0.000001;
    //evaluate the value of the function just at the given point
    double fx= map_functions[function_name].evaluate(arguments);
    //evaluate the value of the function just to the right of the given point
    arguments[0] = a+h;
    double fx1 = map_functions[function_name].evaluate(arguments);
    double slope = (fx1 - fx )/ h;
    return slope;

}


double diff2 (std::string function_name, double a)
{
    std::vector<double> arguments (1);
    arguments[0] = a;
   //define h as tends to 0 somehow
   double h = 0.0000001;
   //value of the func. just to x's right
   arguments[0] = a+h;
   double fx2 = map_functions[function_name].evaluate(arguments);
   //value of the func. just to x's left
   arguments[0] = a-h;
   double fx1 =map_functions[function_name].evaluate(arguments);
   //central approx: f'(x) = (f(x+h) - f(x-h)) / 2h;
   double slope = ( fx2 - fx1 ) / (2*h) ;
   return slope;

}

double diff3 (std::string function_name, double a)
{
    std::vector<double> arguments (1);
    arguments[0] = a;
    //we will evaluate the function at 5 points around a.
    double h = 0.0000001;
    //value of function at distance 2h to the left of the point
    arguments[0] = a - (2*h);
    double fx1=  map_functions[function_name].evaluate(arguments);
    //value of function at distance h to the left of the point
    arguments[0] = a - h;
    double fx2 =  map_functions[function_name].evaluate(arguments);
    //value of function at distance 2h to the right of the point
    arguments[0] = a + h;
    double fx3 =  map_functions[function_name].evaluate(arguments);
    //value of function at distance h to the right of the point
    arguments[0] = a + (2*h);
    double fx4 =  map_functions[function_name].evaluate(arguments);
    double ans = ( fx1 - (8*fx2) + (8*fx3) - fx4 )/ (12*h) ;
    return ans;

}


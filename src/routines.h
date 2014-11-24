#ifndef ROUTINES_H
#define ROUTINES_H

#include "errors.h"
#include "parser.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <time.h>

class Number;
//defines the routines
void def_routines();
//defines the arrays
void def_ndarrays();
//check whether the nan flag from mpfr is set
//this occurs when the function returns a nan
void check_nan_flag();

namespace routines
{
    Number integrate_rm(std::string function_name, Number a, Number b);
    Number integrate_rm_n(std::string function_name, Number a, Number b, Number n);
    Number integrate_rt(std::string function_name, Number a, Number b);
    Number integrate_mc(std::string function_name, Number a, Number b);
    Number integrate2d_rect(std::string function_name, Number a, Number b, Number c, Number d);
    Number integrate2d_type1(std::string function_name, Number a, Number b, std::vector<std::string>aux_arguments);
    Number integrate2d_type2(std::string function_name, Number a, Number b, std::vector<std::string>aux_arguments);
    Number integrate2d_line(std::string function_name, Number a, Number b, std::vector<std::string>aux_arguments);
    Number integrate3d_surf(std::string function_name, Number a, Number b, Number c, Number d, std::vector<std::string>aux_arguments);
    Number integrate3d_line(std::string function_name, Number a, Number b, std::vector<std::string>aux_arguments);
    Number integrate3d_cub(std::string function_name, Number a, Number b, Number c, Number d, Number e, Number f);
    Number newton(std::string function_name, Number a);
    Number bisection(std::string function_name, Number a, Number b);
    Number differentiate(std::string function_name, Number a);
    Number partial_diff2d (std::string function_name , Number x_value, Number y_value , std::vector<std::string>aux_arguments);
    Number partial_diff3d (std::string function_name,  Number x_value, Number y_value , Number z_value, std::vector<std::string>aux_arguments);
}

#endif

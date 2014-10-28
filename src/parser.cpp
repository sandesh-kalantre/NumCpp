#include "parser.hpp"
#include "functions.h"
#include "routines.h"
#include "misc.h"
#include "errors.h"

//Token class constructor
Token::Token()
{
    token_type = NIL;
    operator_id = NOTANOPERATOR;
    operator_precedence = LEVEL0;
    operator_associativity = NONE;
}

//returns the id of the operator according to the enum OPERATOR_ID
enum Token::OPERATOR_ID Token::get_operator_id(const char c)
{
    if(c == '+')
    {
        return PLUS;
    }
    if(c == '-')
    {
        //UNARY_MINUS is determined while parsing in Parser::math_parse
        //by default all minus signs denote subtraction
        return MINUS;
    }
    if(c == '*')
    {
        return MULTIPLY;
    }
    if(c == '/')
    {
        return DIVIDE;
    }
    if(c == '%')
    {
        return MODULUS;
    }
    if(c == 'e' || c == 'E')
    {
        return E;
    }
    if(c == '^')
    {
        return POWER;
    }
    if(c == '!')
    {
        return FACTORIAL;
    }
    return NOTANOPERATOR;
}

enum Token::OPERATOR_PRECEDENCE Token::get_operator_precedence(const char c)
{
    if(c == '+' || c == '-')
    {
        return LEVEL1;
    }
    if(c == '*' || c == '/' || c == '%' || c == 'e' || c == 'E')
    {
        return LEVEL2;
    }
    if(c == '^' || c == '!')
    {
        return LEVEL3;
    }
    return LEVEL0;
}

enum Token::OPERATOR_ASSOCIATIVITY Token::get_operator_associativity(const char c)
{
    if(c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '!' || c == 'e' || c == 'E')
    {
        return LEFT;
    }
    if(c == '^')
    {
        return RIGHT;
    }
    return NONE;
}

std::string::iterator Token::get_token(std::string expr,std::string::iterator it_expr)
{
    //clear the token string
    token.clear();

    //skip over whitespaces
    while(std::isspace(*it_expr))
    {
        it_expr++;
    }

    //check for a semicolon
    if(*it_expr == ';')
    {
        token_type = SEMICOLON;
        token += *it_expr;
        it_expr++;
        return it_expr;
    }

    //check for an equal('=') sign
    if(is_equal_sign(*it_expr))
    {
        token_type = EQUAL_SIGN;
        token += *it_expr;
        it_expr++;
        return it_expr;
    }

    //checks if the char is a function argument separator comma ','
    if(is_comma(*it_expr))
    {
        token_type = COMMA;
        token += *it_expr;
        it_expr++;
        return it_expr;
    }

    //checks whether the characters are a part of a number
    //and stores then in the token
    if(std::isdigit(*it_expr)) //|| *it_expr == '.')
    //'.' part was commented out
    //otherwise it treats strings beginning with . as a number
    //the part was there to handle numbers such as .123 but
    //we have decided that such numbers will not be allowed
    //ALL NUMBERS START WITH A DIGIT
    {
        token_type = NUMBER;
        while(std::isdigit(*it_expr) || *it_expr == '.')
        {
            token += *it_expr;
            it_expr++;
        }
        return it_expr;
    }

    //checks whether the token is an operator among +,-,*,/,%,^,!
    if(is_operator(*it_expr))
    {
        token_type = OPERATOR;
        operator_id = get_operator_id(*it_expr);
        operator_precedence = get_operator_precedence(*it_expr);
        operator_associativity = get_operator_associativity(*it_expr);
        token += *it_expr;
        it_expr++;
        return it_expr;
    }

    //checks whether the token is a left parentheses
    if(is_lparen(*it_expr))
    {
        token_type = LPAREN;
        token += *it_expr;
        it_expr++;
        return it_expr;
    }

    //checks whether the token is a right parentheses
    if(is_rparen(*it_expr))
    {
        token_type = RPAREN;
        token += *it_expr;
        it_expr++;
        return it_expr;
    }

    //checks whether the token is a square left parentheses
    if(is_sq_lparen(*it_expr))
    {
        token_type = SQ_LPAREN;
        token += *it_expr;
        it_expr++;
        return it_expr;
    }

    //checks whether the token is a square right parentheses
    if(is_sq_rparen(*it_expr))
    {
        token_type = SQ_RPAREN;
        token += *it_expr;
        it_expr++;
        return it_expr;
    }


    //checks whether the token is a name either of a variable or a function
    if(std::isalpha(*it_expr) || *it_expr == '_')
    {
        while(std::isalnum(*it_expr) || *it_expr == '_')
        {
            token += *it_expr;
            it_expr++;
        }

        //checks whether the token is a define keyword
        //which the parser uses so as to know that the user is defining
        //a variable or function
        if(is_define(token))
        {
            token_type = DEFINE;
            return it_expr;
        }

        //checks whether the token is show rpn keyword
        if(is_showrpn(token))
        {
            token_type =SHOW_RPN;
            return it_expr;
        }

        //checks whether the token is the name of a routine
        if(is_routine(token))
        {
            token_type = ROUTINE;
            return it_expr;
        }

        //check whether this is a variable or a function
        //function has a left parentheses

        //skip whitespace
        while(std::isspace(*it_expr))
        {
            it_expr++;
        }

        //check if it is a function as a function will have  a left parentheses
        if(is_lparen(*it_expr))
        {
            token_type = FUNCTION;
        }
        else if(is_sq_lparen(*it_expr))
        {
            token_type = NDARRAY;
        }
        else
        {
            token_type = VARIABLE;
        }
        return it_expr;
    }

    //if no token type was matched
    token_type = UNKNOWN;
    std::cout<<UNKNOWN_TOKEN<<std::endl;

    return it_expr;
}

//constructor for the class Parser
Parser::Parser()
{

}

void Parser::parse(std::string expr)
{
    std::string::iterator it_expr = expr.begin();
    Token token;
    it_expr = token.get_token(expr, it_expr);

    //return if the token is UNKNOWN
    if(token.token_type == Token::UNKNOWN)
    {
        return;
    }
    //DEFINITIONS
    //store a variable or a function
    if(token.token_type == Token::DEFINE)
    {
        //once it is define we check whether we are defining
        //a variable or function again by using the Token class
        //which can tell us whether the next token is a variable or a function
        //as function will have an LPAREN after it's name
        it_expr = token.get_token(expr, it_expr);
        if(token.token_type == Token::VARIABLE)
        {
            //store the variable name which will go in the map_varibales
            std::string var_name = token.token;
            //stores the variable value which goes into the map_varibales
            double value;

            it_expr = token.get_token(expr, it_expr);
            if(token.token_type == Token::EQUAL_SIGN)
            {
                Parser math_parser;
                //on the RHS of equal sign is an expression which must be stored as the value of the variable
                math_parser.math_parse(expr, it_expr);
                //the rpn is stored in math_parser.expr_prn
                //use eval_rpn method to generate the value of the rpn and store as the
                //value of the variable
                value = math_parser.eval_rpn(math_parser.expr_rpn);
            }
            else
            {
                //write some ERROR HANDLING ON ACCOUNT OF MISSING = SIGN AFTER DEFINE
            }

            map_variables[var_name] = value;
            return;
        }

        //store a function
        if(token.token_type == Token::FUNCTION)
        {
            //will store the function in map_functions


            Function function;
            function.function_name = token.token;

            //ignore the LPAREN
            it_expr = token.get_token(expr,it_expr);

            while(token.token_type != Token::RPAREN)
            {
                token = Token();
                it_expr = token.get_token(expr, it_expr);
                if(token.token_type == Token::VARIABLE)
                {
                    function.num_arguments++;
                    function.s_arguments.push_back(token.token);
                    //std::cout<<function.s_arguments[0];//<<function.s_arguments[1];
                    function.map_arguments[token.token] = 0;
                }
                else
                {
                    //WRITE ERROR HANDLING FOR NOT AN ARGUMENT
                }
            }
            //arguments have ended and the RPAREN has been been handled
            //This gets the EQUAL_SIGN
            it_expr = token.get_token(expr, it_expr);
            if(token.token_type == Token::EQUAL_SIGN)
            {
                Parser math_parser;
                //on the RHS of equal sign is an expression which must be stored as the value of the function
                math_parser.math_parse(expr, it_expr);
                //the rpn is stored in math_parser.expr_prn
                function.function_rpn = math_parser.expr_rpn;
            }
            else
            {
                //write some ERROR HANDLING ON ACCOUNT OF MISSING = SIGN AFTER DEFINE
            }

            //stores the function in map_functions in map_functions
            map_functions[function.function_name] = function;

            return;
        }

        if(token.token_type == Token::NDARRAY)
        {
            //will store the vector in map_vectors


            ndArray array;
            array.array_name = token.token;

            //ignore the LPAREN
            it_expr = token.get_token(expr,it_expr);

            while(token.token_type != Token::SQ_RPAREN)
            {
                //std::cout<<"bazinga";
                it_expr = token.get_token(expr, it_expr);
                if(token.token_type == Token::NUMBER)
                {
                    array.dim++;
                    array.dim_size.push_back(atof(token.token.c_str()));

                }
                else
                {
                    //WRITE ERROR HANDLING FOR NOT AN ARGUMENT
                }
            }
            std::reverse(array.dim_size.begin(),array.dim_size.end());

            //store the vector
            //initialize the index to zero
            //std::vector<int> index (array.dim,0);
            //the next token is a equal
            it_expr = token.get_token(expr,it_expr);
            //RAISE AN ERORR IF NOT

            /*
            it_expr = token.get_token(expr,it_expr);

            while(!(token.token_type == Token::SQ_RPAREN))
            {
                //std::cout<<"here";
                it_expr = token.get_token(expr,it_expr);
                if(token.token_type == Token::COMMA)
                {
                    continue;
                }
                array.store_value(index,atof(token.token.c_str()));
                (index[0])++;
            }
            //array.show();
            */


            //std::vector<int> index(array.dim,0);
            array.array_def_parse(expr,it_expr);

            map_ndarrays[array.array_name] = array;

            return;
        }
    }

    //SHOW_RPN keyword
    if(token.token_type == Token::SHOW_RPN)
    {
        std::string::iterator it_expr_lparen = it_expr;
        it_expr = token.get_token(expr,it_expr);
        //the next token must be an LPAREN

        if(token.token_type == Token::LPAREN)
        {

            it_expr = token.get_token(expr,it_expr);

            //if the argument is not a function then it is an expression

            math_parse(expr,it_expr_lparen);
            std::queue<Token> expr_rpn_temp = expr_rpn;
            while(!expr_rpn_temp.empty())
            {
                std::cout<<expr_rpn_temp.front().token;
                expr_rpn_temp.pop();
            }
            std::cout<<std::endl;


            //if the argument is a function name,then will show
            //the stored rpn of the function
            if(token.token_type == Token::FUNCTION)
            {
                //check whether the function indeed exists on map_functions
                if(map_functions.find(token.token) != map_functions.end())
                {
                    if(map_functions[token.token].standard == true)
                    {
                        std::cout<<"RPN of standard functions is not stored."<<std::endl;
                    }
                    else
                    {
                        //copy the rpn queue stored in the function
                        std::queue<Token> function_rpn_temp = map_functions[token.token].function_rpn;
                        while(!function_rpn_temp.empty())
                        {
                            std::cout<<function_rpn_temp.front().token;
                            function_rpn_temp.pop();
                        }
                        std::cout<<std::endl;
                    }

                }
                else
                {
                    //THROW AN ERROR THAT THE FUNCTION HAS NOT BEEN DEFINED
                }
                return;
            }



            return;
        }


    }

    //EVALUATIONS
    //variable/function/routine evaluation
    it_expr = expr.begin();
    math_parse(expr, it_expr);

    //DEBUG:PRINT THE RPN AFTER PARSING
    /*
    while(!expr_rpn.empty())
    {
        std::cout<<expr_rpn.front().token;
        expr_rpn.pop();
    }
    std::cout<<std::endl;
    */

    std::cout<<eval_rpn(expr_rpn)<<std::endl;

    return;

}

void Parser::math_parse(std::string expr,std::string::iterator it_expr)
{
    //the stack of operators and functions
    std::stack<Token> operator_stack;
    //this is needed to decide whether minus denotes subtraction
    //or is a unary minus
    Token previous_token;
    Token token = Token();

	while(true)
    {

        previous_token = token;
        it_expr = token.get_token(expr,it_expr);

		if(token.token_type == Token::NUMBER || token.token_type == Token::VARIABLE
		   || token.token_type == Token::NDARRAY)
        {
            expr_rpn.push(token);
            continue;
        }

        if(token.token_type == Token::FUNCTION)
        {
            operator_stack.push(token);
            continue;
        }

        if(token.token_type == Token::ROUTINE)
        {
            operator_stack.push(token);
            continue;
        }

        if(token.token_type == Token::COMMA)
        {
            //THROW SOME ERRORS HERE IF NO LPAREN IS ENCOUNTERED
            while(operator_stack.top().token_type != Token::LPAREN)
            {
                expr_rpn.push(operator_stack.top());
                operator_stack.pop();
            }
            continue;
        }
        if(token.token_type == Token::OPERATOR)
		{
            if(token.operator_id == Token::MINUS)
            {
                //handling the unary minus
                if(previous_token.token_type == Token::NUMBER
                || previous_token.token_type == Token::VARIABLE || previous_token.token_type == Token::FUNCTION
                || previous_token.token_type == Token::ROUTINE)
                {
                    //do nothing as the token by default is stored as a binary minus/subtract
                }
                else
                {
                    token.operator_associativity = Token::RIGHT;
                    token.operator_precedence = Token::LEVEL4;
                    token.operator_id = Token::UNARY_MINUS;
                }
            }

            while(!operator_stack.empty() && operator_stack.top().token_type == Token::OPERATOR)
            {
                if(token.operator_associativity == Token::LEFT && token.operator_precedence <= operator_stack.top().operator_precedence)
                {
                    expr_rpn.push(operator_stack.top());
                    operator_stack.pop();
                }
                else if(token.operator_precedence < operator_stack.top().operator_precedence)
                {
                    expr_rpn.push(operator_stack.top());
                    operator_stack.pop();
                }
				else
				{
					break;
				}
            }

            operator_stack.push(token);
            continue;
        }

        if(token.token_type == Token::LPAREN)
        {
            operator_stack.push(token);
            continue;
        }

        if(token.token_type == Token::RPAREN)
        {
            //THROW SOME ERRORS HERE IF NO LPAREN IS ENCOUNTERED
            while(operator_stack.top().token_type != Token::LPAREN)
            {
                expr_rpn.push(operator_stack.top());
                operator_stack.pop();
            }
            //pop the LPAREN
            operator_stack.pop();

            //is the top of the operator stack it a function
            //then this RPAREN marks the end of the arguments of the function/routine
            //the function/routine so must be pushed to the output queue
            if(!operator_stack.empty() && (operator_stack.top().token_type == Token::FUNCTION || operator_stack.top().token_type == Token::ROUTINE))
            {
                expr_rpn.push(operator_stack.top());
                operator_stack.pop();
            }
            continue;

        }

		if(token.token_type == Token::SEMICOLON)
		{
			break;
		}

		if(token.token_type == Token::UNKNOWN)
		{
            return;
		}

    }

    //if there are still operators on the stack,pop them to the output
    while(!operator_stack.empty())
    {
        expr_rpn.push(operator_stack.top());
        operator_stack.pop();
    }

    return;
}

//will evaluate the rpn tokens passed to in queue<token> expr_rpn
double Parser::eval_rpn(std::queue<Token> expr_rpn)
{
    std::stack<double> number_stack;
    //this is used as argument to routines
    std::string routine_function_name;

    while(!expr_rpn.empty())
    {
        //std::cout<<"here am i"<<std::endl;
        if(expr_rpn.front().token_type == Token::NUMBER)
        {
            number_stack.push(atof(expr_rpn.front().token.c_str()));
            expr_rpn.pop();
            continue;
        }
        if(expr_rpn.front().token_type == Token::VARIABLE)
        {

            number_stack.push(map_variables[expr_rpn.front().token]);
            expr_rpn.pop();
            continue;
        }

        if(expr_rpn.front().token_type == Token::NDARRAY)
        {
            //std::cout<<expr_rpn.front().token;
            map_ndarrays[expr_rpn.front().token].show();
            expr_rpn.pop();
            continue;
        }


        if(expr_rpn.front().token_type == Token::OPERATOR)
        {
            if(expr_rpn.front().operator_id != Token::FACTORIAL && expr_rpn.front().operator_id != Token::UNARY_MINUS)
            {
                //WRITE SOME ERROR HANDLING TO CHECK THAT THERE ATLEAST TWO ELEMENTS ON THE STACK
                //store the top two elements as components of a vector
                std::vector<double>top_two (2);

                top_two[1] = number_stack.top();
                //std::cout<<number_stack.top();
                number_stack.pop();
                top_two[0] = number_stack.top();
                //std::cout<<number_stack.top();
                number_stack.pop();

                if(expr_rpn.front().operator_id == Token::PLUS)
                {
                    number_stack.push(map_functions["ADD"].evaluate(top_two));
                    expr_rpn.pop();
                    continue;
                }
                if(expr_rpn.front().operator_id == Token::MINUS)
                {
                    number_stack.push(map_functions["SUBTRACT"].evaluate(top_two));
                    expr_rpn.pop();
                    continue;
                }

                if(expr_rpn.front().operator_id == Token::MULTIPLY)
                {
                    number_stack.push(map_functions["MULTIPLY"].evaluate(top_two));
                    expr_rpn.pop();
                    continue;
                }
                if(expr_rpn.front().operator_id == Token::DIVIDE)
                {
                    number_stack.push(map_functions["DIVIDE"].evaluate(top_two));
                    expr_rpn.pop();
                    continue;
                }
                if(expr_rpn.front().operator_id == Token::MODULUS)
                {
                    number_stack.push(map_functions["MODULUS"].evaluate(top_two));
                    expr_rpn.pop();
                    continue;
                }
                if(expr_rpn.front().operator_id == Token::E)
                {
                    number_stack.push(map_functions["E"].evaluate(top_two));
                    expr_rpn.pop();
                    continue;
                }
                if(expr_rpn.front().operator_id == Token::POWER)
                {
                    number_stack.push(map_functions["POWER"].evaluate(top_two));
                    expr_rpn.pop();
                    continue;
                }

            }
            //unary operators
            else
            {
                std::vector<double> top (1);
                top[0] = number_stack.top();
                number_stack.pop();
                if(expr_rpn.front().operator_id == Token::FACTORIAL)
                {
                    number_stack.push(map_functions["FACTORIAL"].evaluate(top));
                    expr_rpn.pop();
                    continue;
                }
                if(expr_rpn.front().operator_id == Token::UNARY_MINUS)
                {
                    number_stack.push(map_functions["UNARY_MINUS"].evaluate(top));
                    expr_rpn.pop();
                    continue;
                }

            }

        }

        if(expr_rpn.front().token_type == Token::FUNCTION)
        {

			//ERROR HANDLING TO ENSURE THAT THE FUNCTION EXISTS ON THE Map_functions

			std::queue<Token> temp_expr_rpn;
			temp_expr_rpn = expr_rpn;

			temp_expr_rpn.pop();
			if(temp_expr_rpn.front().token_type == Token::ROUTINE)
			{
				routine_function_name = expr_rpn.front().token;
				expr_rpn.pop();
				continue;
			}


		    //stores the poped arguments from the number_stack
            std::vector<double> arguments;

            //WRITE ERROR HANDLING IF THERE ARE NOT ENOUGH ARGUMENTS ON THE number_stack
            for(int i = 0; i < map_functions[expr_rpn.front().token].num_arguments; i++)
            {
                arguments.push_back(number_stack.top());
                number_stack.pop();
            }
            //the arguments have to be reversed as they are stored in rpn in reverse order
            std::reverse(arguments.begin(),arguments.end());
            //if(arguments.empty()){std::cout<<"arg";}

            number_stack.push(map_functions[expr_rpn.front().token].evaluate(arguments));

            expr_rpn.pop();
            continue;

        }

        if(expr_rpn.front().token_type == Token::ROUTINE)
        {
			std::vector<double> arguments;
            for(int i = 0; i < map_routines[expr_rpn.front().token].num_arguments; i++)
            {
				arguments.push_back(number_stack.top());
                number_stack.pop();
            }
            //the arguments have to be reversed as they are stored in rpn in reverse order
            std::reverse(arguments.begin(),arguments.end());

            number_stack.push(map_routines[expr_rpn.front().token].evaluate(routine_function_name,arguments));
            expr_rpn.pop();
            continue;

        }

        if(expr_rpn.front().token_type == Token::UNKNOWN)
        {
            return 0;
        }
    }

    if(number_stack.size() == 1)
    {
        return number_stack.top();
    }
    else
    {
        //ERROR HANDLING DUE TO TOO MANY VALUES ON THE STACK
    }
    //return 0 by default
    return 0;
}

Function::Function()
{
    function_name = "NIL";
    num_arguments = 0;
    standard = false;
}

void Function::store_rpn(std::queue<Token> rpn)
{
    function_rpn = rpn;
    return;
}

double Function::std_evaluate(std::vector<double> d_arguments)
{
    if(function_name == "ADD")
    {
        return std_functions::add(d_arguments[0], d_arguments[1]);
    }

    if(function_name == "SUBTRACT")
    {
        return std_functions::subtract(d_arguments[0], d_arguments[1]);
    }
    if(function_name == "UNARY_MINUS")
    {
        //std::cout<<"HETE";
        return std_functions::unary_minus(d_arguments[0]);
    }
    if(function_name == "MULTIPLY")
    {
        return std_functions::multiply(d_arguments[0], d_arguments[1]);
    }
    if(function_name == "DIVIDE")
    {
        return std_functions::divide(d_arguments[0], d_arguments[1]);
    }
    if(function_name == "E")
    {
        return std_functions::scientific(d_arguments[0], d_arguments[1]);
    }
    if(function_name == "FACTORIAL")
    {
        return std_functions::factorial(d_arguments[0]);
    }
    if(function_name == "POWER")
    {
        return std_functions::power(d_arguments[0], d_arguments[1]);
    }
    if(function_name == "MODULUS")
    {
        return std_functions::modulus1(d_arguments[0], d_arguments[1]);
    }
    if(function_name == "sin")
    {
        return std_functions::sin(d_arguments[0]);
    }
    if(function_name == "cos")
    {
        return std_functions::cos(d_arguments[0]);
    }
    if(function_name == "tan")
    {
        return std_functions::tan(d_arguments[0]);
    }
    if(function_name == "asin")
    {
        return std_functions::asin(d_arguments[0]);
    }
    if(function_name == "acos")
    {
        return std_functions::acos(d_arguments[0]);
    }
    if(function_name == "atan")
    {
        return std_functions::atan(d_arguments[0]);
    }
    if(function_name == "atan2")
    {
        return std_functions::atan2(d_arguments[0],d_arguments[1]);
    }

    //by deafult it returns zero
    return 0;
}

double Function::evaluate(std::vector<double> d_arguments)
{
    //stores the rpn so that it can be used again
    std::queue<Token> function_rpn_temp = function_rpn;

    //if the function is a standard function,then uses std_evaluate
    if(standard)
    {
        return std_evaluate(d_arguments);
    }

    //copy the d_arguments to the map_arguments
    for(int i = 0; i < num_arguments; i++)
    {
        map_arguments[s_arguments[i]] = d_arguments[i];
    }

    std::stack<double> number_stack;

    while(!function_rpn.empty())
    {
        if(function_rpn.front().token_type == Token::NUMBER)
        {
            number_stack.push(atof(function_rpn.front().token.c_str()));
            function_rpn.pop();
        }
        if(function_rpn.front().token_type == Token::VARIABLE)
        {
            if(map_arguments.find(function_rpn.front().token) != map_arguments.end())
            {
                number_stack.push(map_arguments[function_rpn.front().token]);
                function_rpn.pop();
            }
            else if(map_variables.find(function_rpn.front().token) != map_variables.end())
            {
                number_stack.push(map_variables[function_rpn.front().token]);
                function_rpn.pop();
            }
            else
            {
            //ERROR HANDLING VARIABLE NOT FOUND
            }
        }
        if(function_rpn.front().token_type == Token::OPERATOR)
        {
            //WRITE SOME ERROR HANDLING TO CHECK THAT THERE ATLEAST TWO ELEMENTS ON THE STACK
            if(function_rpn.front().operator_id != Token::FACTORIAL && function_rpn.front().operator_id != Token::UNARY_MINUS)
            {
                //store the top two elements as compenets of a vector
                std::vector<double>top_two (2);
                top_two[1] = number_stack.top();
                number_stack.pop();
                top_two[0] = number_stack.top();
                number_stack.pop();

                if(function_rpn.front().operator_id == Token::PLUS)
                {
                    number_stack.push(map_functions["ADD"].evaluate(top_two));
                    function_rpn.pop();
                    continue;
                }
                if(function_rpn.front().operator_id == Token::MINUS)
                {
                    number_stack.push(map_functions["SUBTRACT"].evaluate(top_two));
                    function_rpn.pop();
                    continue;
                }
                if(function_rpn.front().operator_id == Token::MULTIPLY)
                {
                    number_stack.push(map_functions["MULTIPLY"].evaluate(top_two));
                    function_rpn.pop();
                    continue;
                }
                if(function_rpn.front().operator_id == Token::DIVIDE)
                {
                    number_stack.push(map_functions["DIVIDE"].evaluate(top_two));
                    function_rpn.pop();
                    continue;
                }
                if(function_rpn.front().operator_id == Token::E)
                {
                    number_stack.push(map_functions["E"].evaluate(top_two));
                    function_rpn.pop();
                    continue;
                }
                if(function_rpn.front().operator_id == Token::POWER)
                {
                    number_stack.push(map_functions["POWER"].evaluate(top_two));
                    function_rpn.pop();
                    continue;
                }
            }
            else
            {
                std::vector<double> top (1);
                top[0] = number_stack.top();
                number_stack.pop();
                if(function_rpn.front().operator_id == Token::FACTORIAL)
                {
                    number_stack.push(map_functions["FACTORIAL"].evaluate(top));
                    function_rpn.pop();
                    continue;
                }
                if(function_rpn.front().operator_id == Token::UNARY_MINUS)
                {
                    number_stack.push(map_functions["UNARY_MINUS"].evaluate(top));
                    function_rpn.pop();
                    continue;
                }
            }

        }

        if(function_rpn.front().token_type == Token::FUNCTION)
        {
            //ERROR HANDLING TO ENSURE THAT THE FUNCTION EXISTS ON THE Map_functions

            //stores the poped arguments from the number_stack
            std::vector<double> arguments;

            //WRITE ERROR HANDLING IF THERE ARE NOT ENOUGH ARGUMENTS ON THE number_stack

            for(int i = 0; i < map_functions[function_rpn.front().token].num_arguments; i++)
            {
                arguments.push_back(number_stack.top());
                number_stack.pop();
            }
            number_stack.push(map_functions[function_rpn.front().token].evaluate(arguments));

            function_rpn.pop();

        }
    }

    function_rpn = function_rpn_temp;

    if(number_stack.size() == 1)
    {
        return number_stack.top();
    }
    else
    {
        //ERROR HANDLING DUE TO TOO MANY VALUES ON THE STACK
    }
    //returns zero by default
    return 0;
}

double Routine::evaluate(std::string function_name,std::vector<double> arguments)
{
	if(routine_name.compare("integrate") == 0)
	{
		return routines::integrate(function_name,arguments[0],arguments[1]);
	}

	if(routine_name.compare("differentiate") == 0)
	{
		return routines::differentiate(function_name,arguments[0]);
	}

	//returns 0 as default
	return 0;
}

ndArray::ndArray()
{
    dim = 0;


}

void ndArray::store_value(std::vector<int> inp_index,double value)
{
    array[inp_index] = value;
    //std::cout<<array[inp_index];
    return;
}

double ndArray::return_value(std::vector<int> index)
{
    //std::cout<<array[index];
    return array[index];
}

void ndArray::show()
{
    /*
    std::cout<<"[ ";
    std::vector<int> inp_index(dim,0);
    //std::cout<<dim;
    //std::cout<<dim_size[0];
    for(inp_index[0] = 0; inp_index[0] < dim_size[0];(inp_index[0])++)
    {
        std::cout<<return_value(inp_index)<<" , ";
    }
    std::cout<<']';
    */
    std::reverse(dim_size.begin(),dim_size.end());

    int current_dim = -1;
    std::map<std::vector<int>,double>::iterator it_array;

    for(it_array = array.begin();it_array != array.end();it_array++)
    {
        std::vector<int> index = it_array -> first;

        if(current_dim == -1)
        {
            std::cout<<" [ ";
            current_dim++;
        }
        while(current_dim < dim && current_dim != dim - 1)
        {

            std::cout<<" [ ";
            if((current_dim + 1) < dim)
            {
                current_dim++;
            }
            else
            {
                break;
            }
        }
        if(index[current_dim] == dim_size[current_dim] - 1)
        {
            std::cout<<it_array -> second;
        }
        else
        {
            std::cout<<it_array -> second<<" , ";
        }

        while(index[current_dim] == dim_size[current_dim] - 1)
        {
            current_dim--;
            if(current_dim == -1 || index[current_dim] == dim_size[current_dim] - 1)
            {
                std::cout<<" ] ";
            }
            else
            {
                std::cout<<" ] ,";
            }
        }

    }
    return;
}

void ndArray::array_def_parse(std::string expr,std::string::iterator it_expr)
{
    /*Recursive implementation SOME BUGS ARE THERE
    it_expr = token.get_token(expr,it_expr);

    if(token.token_type == Token::SQ_LPAREN)
    {
        current_dim++;
        index[current_dim] = 0;
        array_def_parse(token,expr,it_expr,current_dim);
        if(current_dim == -1){return;}
    }

    if(token.token_type == Token::COMMA)
    {
        array_def_parse(token,expr,it_expr,current_dim);
        return;
    }


    while(!(token.token_type == Token::SQ_RPAREN))
    {
        //std::cout<<"here";

        if(token.token_type == Token::COMMA)
        {
            it_expr = token.get_token(expr,it_expr);
            continue;
        }
        store_value(index,atof(token.token.c_str()));

        (index[current_dim])++;
        it_expr = token.get_token(expr,it_expr);
    }



    if(token.token_type == Token::SQ_RPAREN)
    {

        current_dim--;
        if(current_dim == -1){return;}
        index[current_dim]++;
        return;
    }
    return;
    */

    //ITERATIVE IMPLEMENTATION
    Token token;
    std::vector<int> index(dim,0);
    int current_dim = -1;
    while(true)
    {
        it_expr = token.get_token(expr,it_expr);
        //std::cout<<token.token;

        if(token.token_type == Token::SEMICOLON)
        {
            return;
        }

        if(token.token_type == Token::SQ_LPAREN)
        {
            current_dim++;
            index[current_dim] = 0;
            continue;
        }

        if(token.token_type == Token::NUMBER)
        {
            //std::cout<<atof(token.token.c_str());
            store_value(index,atof(token.token.c_str()));
            //std::cout<<array[index];
            index[current_dim]++;
            continue;
        }

        if(token.token_type == Token::COMMA)
        {
            continue;
        }

        if(token.token_type == Token::SQ_RPAREN)
        {
            current_dim--;
            if(current_dim == -1)
            {
                return;
            }
            index[current_dim]++;
            continue;
        }
        if(token.token_type == Token::UNKNOWN)
        {
            return;
        }

    }
    return;
}












#include "parser.hpp"
/*
This file contains the implementation of the class sim_eqn
which is used for solving simultaneous equations
*/

//creating a constructor and destructor for the ojects of this class: 3 arrays, A,B,X.
sim_eqn::sim_eqn(unsigned long m)
{
    n = m;
    A = new Number *[n];
    if ( A == NULL)
    {
        throw MALLOC_ERROR;
    }
    //returns pointer of an array of pointers to 1D arrays
    for(long i = 0; i < n; ++i)
    {
        A[i] = new Number[n];
        if ( A[i] == NULL)
        {
            throw MALLOC_ERROR;
        }
    }
    B = new Number [n];
    if ( B == NULL)
    {
        throw MALLOC_ERROR;
    }
    X = new Number [n];
    if ( X == NULL)
    {
        throw MALLOC_ERROR;
    }
    else
    {
        //put some default value for X[i]
        for(long i=0; i < n; i++)
        {
            X[i] = Number(0.0);
        }
    }
    return;
}

sim_eqn::sim_eqn()
{
    //is default constructor really necessary here?
    //for default: 2x2 matrix
    A = new Number *[2];
    if(A == NULL)
    {
        throw MALLOC_ERROR;
    }
    //returns pointer of an array of pointers to 1D arrays
    for(unsigned long i = 0; i < 2; ++i)
    {
        A[i] = new Number[2];
        if(A[i] == NULL)
        {
            throw MALLOC_ERROR;
        }
    }

    B = new Number [2];
    if ( B == NULL)
    {
        throw MALLOC_ERROR;
    }
    X = new Number [2];
    if ( X == NULL)
    {
        throw MALLOC_ERROR;
    }
    return;
}


//create a get function for the answer
Number* sim_eqn::get()
{
    return X;
}
void sim_eqn::set(ndArray _A,ndArray _B)
{
    _A.set2d(A);
    _B.set1d(B);

}
void sim_eqn::solve(long start)
{
    long i, j, k;
    Number temp;

    if (A[start][start] == Number(0.0))
    {
        //row transformation so that A[0][0]!= 0
        for(i=start+1; i<n; i++)
        {
            if(A[i][start] == Number(0.0))
            {
                continue;
            }
            else
            {
                //exchange the 0th and ith row of matrices A and B
                for(j=start; j<n; j++)
                {
                    A[start][j] = A[start][j] + A[i][j];
                    A[i][j] = A[start][j] - A[i][j];
                    A[start][j] = A[start][j] - A[i][j];
                }
             B[start] = B[start] + B[i];
             B[i] = B[start] - B[i];
             B[start] = B[start] - B[i];
            }
            break;

        }
    }
    if(A[start][start] == Number(0.0))
    {
        std::cout<<"Variable "<<start<<" cannot be found."<<std::endl;
        std::cout<<"Either Infinite solutions for "<< start<<" variable or no solution"<<std::endl;
        if (start == n-1)
        {
            return;
        }
        else
        {
            solve(start +1);
            Number check = Number(0.0);
            for (i= start +1 ; i < n; i++)
            {
                check += A[start][i] * X[i];
            }
            if (check == B[start])
            {
                std::cout<<"There are infinite solns for 0th x"<< std::endl;
            }
            else
            {

                std::cout<<"There is no solution possible. The equations contradict."<<std::endl;
                return;
            }
            /*
            SPECIAL CASE:
            we can try for the other variables though...recursive solving
            solve(start+1);
            must check if new values satisfy row start eqn. before printing but after calculating.
            will also have to think of terminating case
            */

        }

    }

    else
    {

        for (k = start; k < n-1; k++)
        {
            for(i = k + 1; i < n; i++)
            {
                if(A[k][k] != 0)
                {
                    temp= A[i][k];
                    for(j = 0; j <n; j++)
                    {
                        A[i][j] = A[i][j] - ( A[k][j] * ( temp / A[k][k]));
                    }
                    B[i] = B[i] - (( temp / A[k][k] ) * B[k] ) ;
                    //conversion to upper triangular matrix.
                }
                else
                {
                    solve(start + 1);
                }
            }
        }

        Number Divisor = Number(0.0);
        long stopPos = start - 1;
        //-1 just helps us in the loop for printing values.
        for(i = start;i < n;i++)
        {
            if ( A[i][i] != Number(0.0))
            {
                Divisor = A[i][i];
                /*
                to make the diagonal element unity, we should divide it by itself and all other elements in that row by the same element.
                CAUTION:but this cannot be done if A[i][i]==0
                in an upper triangular matrix, if one of the diagonal elements becomes zero, the whole determinant becomes 0.
                */

                A[i][i] = Number(1.0);
                for (j = i+1; j < n; j++ )
                {
                    A[i][j] /= Divisor;
                }
                //we must also do it for the other side on the equality.
                B[i] /= Divisor;

            }
            else
            {
                std::cout<<"Variable "<< i<< " cannot be found out from these equations"<<std::endl;
                std::cout<< "So even ";
                for(j = 0; j < i; j++)
                {
                    std::cout<<"variable "<<j<<" , ";
                }
                std::cout<<" cannot be found either."<< std::endl;
                stopPos = i;
            }

        }

        for (i = n-1; i > stopPos ; i--)
        {
            X[i] = B[i];
            for (j = n-1; j > i; j--)
            {
            // These variables would be updated by the end of the loop.
            X[i] -= A[i][j]* X[j];
            }
            //now we'll just print out the values of the variables
        }

    }//end of else block
    return;
}

sim_eqn::~sim_eqn()
{
    long i;
    //after printing all the values of the variables, we should delete the arrays to prevent leaks
    for (i=0; i<n; i++)
    {
        if ( A[i] != NULL)
        {
            delete [] A[i];
        }
    }
    if ( B != NULL)
    {
        delete [] B;
    }
    if ( X != NULL)
    {
        delete [] X;
    }
    return;
}

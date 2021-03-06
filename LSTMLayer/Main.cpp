// LSTMLayer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MatrixD.h"
#include "Parallel.h"
#include <string>
#include <locale>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "LSTMLayer.h"
#include "FullConnectionLayer.h"


using std::cout;
using std::cin;
using std::endl;
using std::string;



string ToLower( const string &input )
{
	string res = input;
	for (char & re : res)
	{
		re = tolower( re );
	}
	return res;
}

double getRand()
{
	static bool running = false;
	while ( running )
	{
	}
	running = true;
	double res = (rand() / (double)RAND_MAX) - (rand() / (double)RAND_MAX);
	running = false;
	return res;
}

std::ostream&  operator<<( std::ostream &out , const MatrixD &d )
{
	using std::stringstream;
	using std::string;
	int rows = d.Rows();
	int cols = d.Columns();
	for ( int i = 0; i < rows; ++i )
	{
		for ( int j = 0; j < cols; ++j )
		{

			out <<std::fixed << std::setprecision(2) << d[i][j] << "\t";
		}
		out << endl;
	}
	return out;
}

int main()
{
	srand( 356441412 );

	MatrixD a( 2000 , 2000);
	MatrixD b( 2000 , 2000 );
	int rows;
	int cols;

	rows = a.Rows();
	cols = a.Columns();

	for ( int i = 0; i < rows; ++i )
	{
		for ( int j = 0; j < cols; ++j )
		{
			a[i][j] = i + j;
		}
	}
	rows = b.Rows();
	cols = b.Columns();

	for ( int i = 0; i < rows; ++i )
	{
		for ( int j = 0; j < cols; ++j )
		{
			if ( i == j )
			{
				b[i][j] = 1.0;
			} else
			{
				b[i][j] = 0.0;
			}
		}
	}


	a.AplyFunction([](double d)->double { return getRand(); });

	b.ApliedFunction([](double d)->double { return getRand(); });

	MatrixD res = a * b;
	MatrixD resp = a + b;
	MatrixD resm = a - b;
	MatrixD resh = a % b;


    return 0;
}


/*
 * math_lib.cpp
 *
 *  Created on: Apr 26, 2015
 *  Author: Arthur Laks

 * Contains the implemenations of functions used to invert and multiply matrices in Zn.
 */
#include "math_lib.h"
#include <algorithm>
using std::vector;
using std::swap;
using std::transform;

//The anonymous namespace contains functions that are used internally in the module.

//The following code is from http://www.pagedon.com/extended-euclidean-algorithm-in-c/my_programming/
namespace{
/*****************************************************************************
 * Finds an integer i such that ai [=] 1 (mod b)
 * Since we know a and b it uses Euclidean Algorithm to find the inverse of a
 * Note ai + bj = 1 Considering i is inverse of a and j is inverse of b
 *******************************************************************************/
int find_inverse(int a, int b){
	int n = b;
	int x[3];
	int y[3];
	int quotient  = a / b;
	int remainder = a % b;

	x[0] = 0;
	y[0] = 1;
	x[1] = 1;
	y[1] = quotient * -1;

	int i = 2;
	for (; (b % (a%b)) != 0; i++){
		a = b;
		b = remainder;
		quotient = a / b;
		remainder = a % b;
		x[i % 3] = (quotient * -1 * x[(i - 1) % 3]) + x[(i - 2) % 3];
		y[i % 3] = (quotient * -1 * y[(i - 1) % 3]) + y[(i - 2) % 3];
	}

	//x[i - 1 % 3] is inverse of a
	//y[i - 1 % 3] is inverse of b
	int inverse = x[(i - 1) % 3];
	//Make sure that the inverse is positive.
	if(inverse < 0)
		inverse += n;
	return inverse;
}

//Obtained from http://www.math.wustl.edu/~victor/mfmm/compaa/gcd.c
int
gcd ( int a, int b )
{
  int c;
  while ( a != 0 ) {
     c = a; a = b%a;  b = c;
  }
  return b;
}

//Adds the row beginning with dest_begin to a multiple of the row beginning with source_begin in Zn,
//and replaces the row beginning with dest_begin with the result.
void add_rows(vector<int>::iterator source_begin,vector<int>::iterator source_end,
		vector<int>::iterator dest_begin,int multiply_by,int n){
	transform(source_begin,source_end,dest_begin,dest_begin,
			[&](int a,int b){return (b + multiply_by * a) % n;});
}
}

//Inverts a matrix in Zn.  The parameter is turned into the inverse.  Returns true if the matrix is invertible.
//The argument represents the left-hand-side of the augmented matrix.
bool invert(Matrix& lhs,unsigned n){
	unsigned dimension = lhs.size();
	Matrix rhs(lhs.size(),vector<int>(lhs.size(),0));	//Represents the right-hand-side of the augmented matrix.
	//All row operations will be performed on both matrices, and then they will be swapped.

	//Initialize to an identity matrix.
	for(unsigned i = 0;i < dimension;++i){
	  rhs[i][i] = 1;
	}

	//For each row.
	for(unsigned row = 0;row < dimension;++row){
		//Make sure the element in the pivot position is 1.
		//If the element in the pivot position is 0 or does not have an inverse mod n,
		//swap it with the first one that is.
		if(!lhs[row][row] || gcd(lhs[row][row],n) != 1){
			//Find the first row that has a number that is invertible mod n in the position below the pivot position.
			auto nonzero_row = std::find_if(lhs.begin() + row + 1,lhs.end(),
					[&](const vector<int>& c_row){return c_row[row] && gcd(c_row[row],n) == 1;});
			//If such a row does not exist, the matrix is singular, so return false.
			if(nonzero_row == lhs.end()){
				return false;
			}
			//Swap the current row with that row in both parts of the augmented matrix.
			std::iter_swap(lhs.begin() + row,nonzero_row);

			swap(rhs[row],rhs[nonzero_row - lhs.begin()]);
		}

		//Multiply the row by the inverse of the element at the pivot position to turn the
		//pivot position to 1.

		int inverse = find_inverse(lhs[row][row],n);
		//All of the previous columns of the lhs are assumed to be zero, so start from row.
		transform(lhs[row].begin()+ row,lhs[row].end(),lhs[row].begin() + row,
				[&](int element){return (element * inverse) % n;});

		//Perform the same operation for the rhs.
		transform(rhs[row].begin(),rhs[row].end(),rhs[row].begin(),
				[&](int element){return (element * inverse) % n;});

		//Turn all of the columns under the pivot position to zero.
		for(unsigned c_row = row + 1;c_row < dimension;++c_row){
			int multiply_by = n - lhs[c_row][row];
			add_rows(lhs[row].begin() + row,lhs[row].end(),lhs[c_row].begin() + row,
					multiply_by,n);
			add_rows(rhs[row].begin(),rhs[row].end(),rhs[c_row].begin(),multiply_by,n);
		}
	}

	//Now that the matrix is in echelon form, reduce it to reduced echelon form by turning
	//all the entries above pivot positions to zero, starting from the bottom row.
	for(int row = dimension - 1;row >= 0;--row){

		for(int c_row = 0;c_row < row;++c_row){
			unsigned int multiply_by = n - lhs[c_row][row];
			add_rows(lhs[row].begin() + row,lhs[row].end(),lhs[c_row].begin() + row
					,multiply_by,n);
			add_rows(rhs[row].begin(),rhs[row].end(),rhs[c_row].begin(),multiply_by,n);
		}
	}

	swap(rhs,lhs);
	return true;
}

//Multiplies the two matrices in Zn, returning the result.
Matrix multiply(const Matrix& lhs,const Matrix& rhs,unsigned int n){
    Matrix result(lhs.size(),vector<int>(rhs.size()));
    //For each row i and column j of the resulting matrix, assign in to the inner product of row i of rhs and column j
    //of lhs.
    for(unsigned int i = 0; i < lhs.size(); ++i) {
        for(unsigned int j = 0; j < rhs.size(); ++j) {
            result[i][j] = std::inner_product(lhs[i].begin(),lhs[i].end(),rhs.begin(),0,
                        [&] (int a,int b) {return (a + b) % n;},
                        [&](int a,vector<int> row) {return (a * row[j]) % n;});
        }
    }
    return result;
}

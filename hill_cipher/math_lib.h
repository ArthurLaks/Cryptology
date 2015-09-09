/*
 * math_lib.h
 *
 *  Created on: Apr 26, 2015
 *      Author: Arthur Laks
 *  Contains the declarations of functions used to
 */

#ifndef MATH_LIB_H
#define MATH_LIB_H

#include <vector>
typedef std::vector<std::vector<int> > Matrix;

//Turns the matrix into its inverse, mod the second argument.
//Returns true if the matrix is invertible, otherwise false.
bool invert(Matrix&,unsigned);

//Multiplies the first matrix by the second matrix mod the third argument, and returns resulting matrix.
Matrix multiply(const Matrix&,const Matrix&,unsigned);

#endif /* MATRIX_INVERTER_H_ */

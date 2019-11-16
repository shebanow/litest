/**
 * @file hwmult.cpp
 * @author m.shebanow
 * @date 11/09/2019
 * @brief HW multiplier.
 */
#include <stdio.h>
#include "vector.h"
#include "matrix.h"
#include "hwmult.h"

// Matrix Multiply: returns number of non-zero multiplies
template <int P>
Vector_t<int8_t> hwMatrixMultiply(const Matrix_t<int8_t>& matrix, const Vector_t<int8_t>& in) {	
	assert(matrix.width() == P && matrix.height() == P && in.width() == P);
	return matrix.mm(in);
}

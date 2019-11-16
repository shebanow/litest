/**
 * @file hwmult.h
 * @author m.shebanow
 * @date 11/09/2019
 * @brief model for HW multiplier.
 */
#ifndef _HWMULT_H_
#define _HWMULT_H_
#include <stdio.h>
#include "vector.h"
#include "matrix.h"

// Matrix Multiply: returns vector result
template <int P>
Vector_t<int8_t> hwMatrixMultiply(const Matrix_t<int8_t>& matrix, const Vector_t<int8_t>& in);

#endif /* _HWMULT_H_ */
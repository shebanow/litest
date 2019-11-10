/**
 * @file tensor.cpp
 * @author m.shebanow
 * @date 11/09/2019
 * @brief Tensor class implementation.
 */
#include <stdio.h>
#include <assert.h>
#include "tensor.h"

/******************************* 1D (Vector) ***********************************/
//!< 1D (Vector) Constructor
template <typename T> 
Tensor<T, 1>::Tensor(int _W): W(_W) { 
	data = new T[W]; 
	for (int i = 0; i < W; i++)
		data[i] = 0;
}

//!< 1D (Vector) Copy Constructor
template <typename T> 
Tensor<T, 1>::Tensor(const Tensor<T, 1>& t): W(t.W) { 
	data = new T[W]; 
	for (int i = 0; i < W; i++)
		data[i] = t.data[i];
}

//!< 1D (Vector) Destructor
template <typename T>
Tensor<T, 1>::~Tensor() {
	delete data;
}

//!< 1D (Vector) Multiply vector * scalar
template <typename T> 
Tensor<T, 1>& Tensor<T, 1>::operator*= (const T scalar) {
	for (int i = 0; i < W; i++)
		data[i] *= scalar;
	return *this;
}

//!< 1D (Vector) Dot product
template <typename T> 
T Tensor<T, 1>::operator* (const Tensor<T, 1>& v) {
	T accumulator = 0;

	assert(W == v.W);
	for (int i = 0; i < W; i++)
		accumulator += data[i] * v.data[i];
	return accumulator;
}

//!< 1D (Vector) Add a vector to another
template <typename T> 
Tensor<T, 1>& Tensor<T, 1>::operator+= (const Tensor<T, 1>& v) {
	assert(W == v.W);
	for (int i = 0; i < W; i++)
		data[i] += v.data[i];
	return *this;
}

/******************************* 2D (Matrix) ***********************************/
//!< Constructor for 2D (Matrix) tensor class
template <typename T> 
Tensor<T, 2>::Tensor(int _W, int _H): W(_W), H(_H), len(_W * _H) { 
	data = new T[len]; 
	for (int i = 0; i < len; i++)
		data[i] = 0;
}

//!< Copy constructor for 2D (Matrix) tensor class
template <typename T> 
Tensor<T, 2>::Tensor(const Tensor<T, 2>& t) : W(t.W), H(t.H), len(t.W * t.H) { 
	data = new T[len]; 
	for (int i = 0; i < len; i++)
		data[i] = t.data[i];
}

//!< destructor for 2D (Matrix) tensor class
template <typename T> 
Tensor<T, 2>::~Tensor() { 
	delete data;
}

/*********************************** 3D ****************************************/
//!< Constructor for 3D tensor class
template <typename T> 
Tensor<T, 3>::Tensor(int _W, int _H, int _D): W(_W), H(_H), D(_D), len(_W * _H * _D) { 
	data = new T[len]; 
	for (int i = 0; i < len; i++)
		data[i] = 0;
}

//!< Copy constructor for 3D tensor class
template <typename T> 
Tensor<T, 3>::Tensor(const Tensor<T, 3>& t) : W(t.W), H(t.H), D(t.D), len(t.W * t.H * t.D)  { 
	data = new T[len]; 
	for (int i = 0; i < len; i++)
		data[i] = t.data[i];
}

//!< destructor for 3D tensor class
template <typename T> 
Tensor<T, 3>::~Tensor() { 
	delete data;
}

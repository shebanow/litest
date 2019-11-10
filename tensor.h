/**
 * @file Tensor.h
 * @author m.shebanow
 * @date 11/09/2019
 * @brief model for HW multiplier.
 */
#pragma once
#ifndef _TENSOR_H_
#define _TENSOR_H_
#include <assert.h>

using namespace std;

// basic tensor class template
template <typename T, int N> 
class Tensor;

// 1-dimensional tensor (aka vector) specialization
template <typename T>
class Tensor<T, 1> {
public:
	Tensor(int _W = 1); 								//!< Contructor: dimension is optional and defaults to 1
	Tensor(const Tensor& t);							//!< Copy constructor
	~Tensor(); 											//!< Destructor: release allocated space

	//!< return a reference to a tensor member
	inline T& operator()(int i) { assert(i >= 0 && i < W); return data[i]; }

	//!< return vector length
	inline const int length() { return W; }

	// basic vector ops
	Tensor<T, 1>& operator*= (const T scalar); 			//!< Multiply a vector by a scalar
	T operator* (const Tensor<T, 1>& v);				//!< Dot product
	Tensor<T, 1>& operator+= (const Tensor<T, 1>& v);	//!< Add a vector to another

private:
	T *data;
	const int W;
};

// 2-dimensional tensor (aka matrix) specialization
template <typename T>
class Tensor<T, 2> {
public:
	Tensor(int _W = 1, int _H = 1); 					//!< Constructor: dimensions are optional and default to 1
	Tensor(const Tensor<T,2>& t); 						//!< Copy Constructor
	~Tensor(); 											//!< Destructor: release allocated space

	// dimension methods
	inline const int width() { return W; }
	inline const int height() { return H; }
	inline const int length() { return len; }

	//!< return a reference to a tensor member
	inline T& operator()(int i, int j) {  assert(i >= 0 && i < W && j >= 0 && j < H); return data[j * W + i]; }

private:
	T *data;
	const int len;
	const int W, H;
};

// 3-dimensional tensor specialization
template <typename T>
class Tensor<T, 3> {
public:
	Tensor(int _W = 1, int _H = 1, int _D = 1);			//!< Constructor: dimensions are optional and default to 1
	Tensor(const Tensor<T, 3>& t);						//!< Copy Constructor
	~Tensor(); 											//!< Destructor: release allocated space

	// dimension methods
	inline const int width() { return W; }
	inline const int height() { return H; }
	inline const int depth() { return D; }
	inline const int length() { return len; }
	
	//!< return a reference to a tensor member
	inline T& operator()(int i, int j, int k) { assert(i >= 0 && i < W && j >= 0 && j < H && k >= 0 && k < D); return data[(k * H + j) * W + i]; }

private:
	T *data;
	const int len;
	const int W, H, D;
};

#endif // _TENSOR_H_

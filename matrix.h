/**
 * @file matrix.h
 * @author m.shebanow
 * @date 11/09/2019
 * @brief basic 2D matrix data type.
 */
#ifndef _MATRIX_H_
#define _MATRIX_H_
#include <assert.h>
#include <string>
#include "vector.h"

using namespace std;

template <typename T>
class Matrix_t {
public:
	//!< Constructor 
	Matrix_t(int _W = 1, int _H = 1) : W(_W), H(_H), len(_W * _H) {
		data = new T[len];
		T *dst = data;

		for (int i = 0; i < W; i++)
			for (int j = 0; j < H; j++)
				*dst++ = 0;
	}

	//!< Copy constructor
	Matrix_t(const Matrix_t<T>& m) : W(m.W), H(m.H), len(m.W * m.H) { 
		data = new T[len];
		T *src = m.data;
		T *dst = data;

		for (int i = 0; i < W; i++)
			for (int j = 0; j < H; j++)
				*dst++ = *src++;
	}

	virtual ~Matrix_t() { delete data; }

	// dimension methods
	inline const int width() const { return W; }
	inline const int height() const { return H; }
	inline const int length() const { return len; }

	//!< return a reference to a tensor member
	inline T& operator()(int i, int j) {  assert(i >= 0 && i < W && j >= 0 && j < H); return data[j * W + i]; }

	// Matrix multiply on vector, "result = m * v"
	Vector_t<T> mm(const Vector_t<T>& v) const {
		Vector_t<T> result(H);

		assert(W = v.W);
		for (int j = 0; j < H; j++) {
			T sum = 0;
			T *src1 = &data[j * W];
			T *src2 = v.data;

			for (int i = 0; i < W; i++)
				sum += *src1++ * *src2++;
			result.data[j] = sum;
		}
		return result;
	}

	// method to extract row
	Vector_t<T> rowVector(const int j) const{
		Vector_t<T> result(W);
		T *dst = result.data;
		T *src = &data[j * W];

		for (int i = 0; i < W; i++)
			*dst++ = *src++;
		return result;
	}

	// method to extract column
	// BUG: reverse row/col?
	Vector_t<T> colVector(const int i) const {
		Vector_t<T> result(H);
		T *dst = result.data;
		T *src = &data[i];

		for (int j = 0; j < H; j++) {
			*dst++ = *src;
			src += W;
		}
		return result;
	}

	//!< Generate a string with matrix geoemtry
	string operator() () const {
	    static char buffer[32];

	    sprintf(buffer, "[%d,%d]", W, H);
	    return string(buffer);
	}

private:
	T *data;
	const int len;
	const int W, H;
	
	template <typename TT> friend class Tensor_t;
};

#endif // _MATRIX_H_
